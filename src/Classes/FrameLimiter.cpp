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

#pragma once

#include <Classes/FrameLimiter.h>
#include <Windows.h>
#include <atlbase.h>
#include <string>

FrameLimiter::FrameLimiter(int targetFPS): 
    m_targetFPS(targetFPS), m_ticksAccumulator(0), m_frameCount(0), m_averageFPS(0),
    m_overSleepDuration(0), m_elapsedTime(0), m_targetFrameTime(1000000 / targetFPS) 
{
    QueryPerformanceFrequency((LARGE_INTEGER*)&m_QPCFrequency);
    m_frameStart = m_frameEnd = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&m_frameStart);
}

unsigned long long FrameLimiter::GetElapsedMicroseconds(unsigned long long startCount, unsigned long long endCount) 
{
    INT64 elapsedMicroseconds = endCount - startCount;
    elapsedMicroseconds *= 1000000;
    elapsedMicroseconds /= m_QPCFrequency;
    return elapsedMicroseconds;
}

void FrameLimiter::SleepUntilNextFrame() 
{
    while (m_elapsedTime < m_targetFrameTime) 
    {
        if ((m_elapsedTime + m_overSleepDuration) >= m_targetFrameTime) 
        {
            m_overSleepDuration -= m_targetFrameTime - m_elapsedTime;
            break;
        }
        Sleep(1);
        QueryPerformanceCounter((LARGE_INTEGER*)&m_frameEnd);
        m_elapsedTime = GetElapsedMicroseconds(m_frameStart, m_frameEnd);
        if (m_elapsedTime > m_targetFrameTime)
            m_overSleepDuration += m_elapsedTime - m_targetFrameTime;
    }
}

void FrameLimiter::WaitForFrame() 
{
    QueryPerformanceCounter((LARGE_INTEGER*)&m_frameEnd);
    m_elapsedTime = GetElapsedMicroseconds(m_frameStart, m_frameEnd);
    SleepUntilNextFrame();

    QueryPerformanceCounter((LARGE_INTEGER*)&m_frameEnd);
    m_ticksAccumulator += m_frameEnd - m_frameStart;
    m_frameCount++;

    if ((m_frameCount % m_targetFPS) == 0) 
    {
        m_averageFPS = ((m_QPCFrequency * m_targetFPS) + (m_ticksAccumulator - 1)) / m_ticksAccumulator;
        m_ticksAccumulator = 0;
        m_frameCount = 0;
    }

    m_frameStart = m_frameEnd;
}

void FrameLimiter::DisplayFPS() 
{
    QueryPerformanceCounter((LARGE_INTEGER*)&m_frameEnd);
    INT64 currentTime = m_frameEnd;

    if (currentTime - m_lastFPSUpdateTime >= m_QPCFrequency) 
    {
        TCHAR outstr[1024];
        _sntprintf_s(outstr, _countof(outstr), _TRUNCATE, _T("FPS: %lld\n"), m_averageFPS);
        OutputDebugStringA(outstr);

        m_lastFPSUpdateTime = currentTime;
    }
}