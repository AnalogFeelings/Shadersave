// Shadersave - A screensaver that can run Shadertoy shaders locally.
// Copyright (C) 2024-2024 Analog Feelings
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

#include <windows.h>

namespace Globals
{
	inline HWND MainWindow;
	inline HDC DeviceContext;
	inline HGLRC GlRenderContext;
	inline RECT ClientRect;

	inline std::string LastError;

	inline UINT BufferATexture;
	inline UINT BufferBTexture;
	inline UINT BufferCTexture;
	inline UINT BufferDTexture;
}