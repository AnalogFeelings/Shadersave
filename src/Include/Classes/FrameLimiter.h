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

#include <stdint.h>
#include <Windows.h>

class FrameLimiter 
{
public:
	explicit FrameLimiter(int targetFPS);
	auto Start() -> void;
	auto End() -> void;
	auto SetTimeLimit(uint64_t microSeconds) -> void;
	auto GetElapsedUs() -> uint64_t;
	auto GetRemainingUs() -> uint64_t;

private:
	auto GetPerformanceCounter() -> uint64_t;
	auto GetPerformanceCounterFrequency() -> uint64_t;

	uint64_t m_perfCounterStart;
	uint64_t m_perfCounterEnd;
	uint64_t m_perfCounterLimit;

	static uint64_t sm_qpcFreq;
};