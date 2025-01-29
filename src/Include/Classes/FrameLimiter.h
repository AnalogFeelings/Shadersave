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

#include <Windows.h>

#pragma once

class FrameLimiter 
{
public:
	explicit FrameLimiter(int targetFPS = 60);
	auto WaitForFrame() -> void;
	auto DisplayFPS() -> void;

private:
	// Maybe consider using Windows definitions.
	int m_targetFPS;
	unsigned long long m_QPCFrequency;
	unsigned long long m_frameStart, m_frameEnd;
	unsigned long long m_averageFPS, m_ticksAccumulator;
	unsigned long long m_elapsedTime, m_overSleepDuration;
	unsigned long long m_frameCount;
	unsigned long long m_targetFrameTime;
	unsigned long long m_lastFPSUpdateTime;

	auto SleepUntilNextFrame() -> void;
	auto GetElapsedMicroseconds(unsigned long long startCount, unsigned long long endCount) -> unsigned long long;
};
