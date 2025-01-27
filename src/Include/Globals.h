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

#include <Defines.h>

#include <windows.h>
#include <string>
#include <unordered_set>

namespace Globals
{
	inline HWND MainWindow;
	inline HDC DeviceContext;
	inline HGLRC GlRenderContext;
	inline RECT ClientRect;

	inline HANDLE FrameTimer;
	inline LARGE_INTEGER FrameDueTime;
	inline int FrameDurationUs;

	inline std::string LastError;

	inline std::unordered_set<std::string> ValidBindings =
	{
		BUFFER_A,
		BUFFER_B,
		BUFFER_C,
		BUFFER_D
	};
}
