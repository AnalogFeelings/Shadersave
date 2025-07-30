// Shadersave - A screensaver that can run Shadertoy shaders locally.
// Copyright (C) 2024 Analog Feelings
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
// 
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <Classes/FrameLimiter.h>

FrameLimiter::FrameLimiter(int32_t targetFPS)
{
    m_qpcFreq = GetPerformanceCounterFrequency();
    m_frameTime = m_qpcFreq / targetFPS;

    m_perfCounterStart = GetPerformanceCounter();
    m_perfCounterLimit = m_perfCounterStart + m_frameTime;
}

auto FrameLimiter::StartTimer() -> void
{
    m_perfCounterStart = GetPerformanceCounter();
}

auto FrameLimiter::EndTimer() -> void
{
    m_perfCounterEnd = GetPerformanceCounter();
    m_perfCounterLimit = m_perfCounterStart + m_frameTime;

    if (m_perfCounterLimit < m_perfCounterEnd)
        m_perfCounterLimit = m_perfCounterEnd;
}

auto FrameLimiter::LimitReached() -> bool
{
    return (GetPerformanceCounter() >= m_perfCounterLimit);
}

auto FrameLimiter::WaitForNextFrame() -> void
{
    while (!LimitReached())
    {
        uint64_t remainingUs = GetRemainingUs();

        // We can sleep for most of the time so that we're not hogging the CPU.
        if (remainingUs > 2000)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(remainingUs - 1000));
        }
        else
        {
            while (!LimitReached()) { _mm_pause(); }
            break;
        }
    }
}

auto FrameLimiter::GetPerformanceCounter() -> uint64_t
{
    LARGE_INTEGER retval;
    QueryPerformanceCounter(&retval);
    return retval.QuadPart;
}

auto FrameLimiter::GetPerformanceCounterFrequency() -> uint64_t
{
    LARGE_INTEGER retval;
    QueryPerformanceFrequency(&retval);
    return retval.QuadPart;
}

auto FrameLimiter::GetRemainingUs() -> uint64_t
{
    uint64_t perfCountNow = GetPerformanceCounter();
    uint64_t nPerformanceCountLeft = perfCountNow < m_perfCounterLimit ? m_perfCounterLimit - perfCountNow : 0;

    return (nPerformanceCountLeft * 1'000'000 / m_qpcFreq);
}

auto FrameLimiter::SetHighPrecisionTiming(bool enable) -> void
{
    static bool initialized = false;
    if (enable && !initialized)
    {
        timeBeginPeriod(1);
        initialized = true;
    }
    else if (!enable && initialized)
    {
        timeEndPeriod(1);
        initialized = false;
    }
}