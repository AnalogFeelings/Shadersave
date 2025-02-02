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

// We have to do this so that the linker includes the multimedia lib.
// This supports the use of timeBegin/EndPeriod for Win32 timers.
#pragma comment(lib, "winmm.lib")
#include <Windows.h>
#include <timeapi.h>
#include <stdint.h>
#include <chrono>
#include <thread>

class FrameLimiter 
{
public:
	FrameLimiter() : 
		m_perfCounterStart(0), m_perfCounterEnd(0), m_perfCounterLimit(0), 
		m_qpcFreq(0), m_frameTime(0) {}

	FrameLimiter(int targetFPS);
	auto StartTimer() -> void;
	auto EndTimer() -> void;
	auto WaitForNextFrame() -> void;
	auto SetHighPrecisionTiming(bool enable) -> void;

private:
	auto GetPerformanceCounter() -> uint64_t;
	auto GetPerformanceCounterFrequency() -> uint64_t;
	auto GetRemainingUs() -> uint64_t;
	auto LimitReached() -> bool;

	uint64_t m_perfCounterStart;
	uint64_t m_perfCounterEnd;
	uint64_t m_perfCounterLimit;
	uint64_t m_frameTime;
	uint64_t m_qpcFreq;
};