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

FrameLimiter::FrameLimiter(int targetFPS)
{
	m_qpcFreq = GetPerformanceCounterFrequency();
	m_perfCounterStart = GetPerformanceCounter();

	m_perfCounterLimit = m_perfCounterStart + (((1'000'000 / targetFPS) * m_qpcFreq) / 1'000'000);
}

auto FrameLimiter::Start() -> void
{
	m_perfCounterStart = GetPerformanceCounter();
}

auto FrameLimiter::End() -> void
{
	m_perfCounterEnd = GetPerformanceCounter();
}

auto FrameLimiter::LimitReached() -> bool
{
	uint64_t perfCountNow = GetPerformanceCounter();

	if (perfCountNow < m_perfCounterStart)
	{
		m_perfCounterLimit = perfCountNow + (m_perfCounterLimit - m_perfCounterStart);
	}

	return (perfCountNow >= m_perfCounterLimit);
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


auto FrameLimiter::GetElapsedUs() -> uint64_t
{
	return ((m_perfCounterEnd - m_perfCounterStart) * 1'000'000 / m_qpcFreq);
}

auto FrameLimiter::GetRemainingUs() -> uint64_t
{
	uint64_t perfCountNow = GetPerformanceCounter();
	uint64_t nPerformanceCountLeft = perfCountNow < m_perfCounterLimit ? m_perfCounterLimit - perfCountNow : 0;

	return (nPerformanceCountLeft * 1'000'000 / m_qpcFreq);
}