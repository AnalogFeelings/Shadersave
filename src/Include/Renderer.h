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
#include <Defines.h>

namespace Renderer
{
	constexpr float QUAD_VERTICES[12] =
	{
		-1.0f, -1.0f, -0.0f,
		+1.0f, +1.0f, -0.0f,
		-1.0f, +1.0f, -0.0f,
		+1.0f, -1.0f, -0.0f
	};
	constexpr unsigned int QUAD_INDICES[6] =
	{
		0, 1, 2,
		0, 3, 1
	};

	auto InitContext(HWND hWnd, HDC& deviceContext, HGLRC& glRenderContext) -> bool;
	auto InitRenderer(int viewportWidth, int viewportHeight, const RenderSettings& settings) -> bool;
	auto UninitializeRenderer() -> void;

	auto DoRender(HDC deviceContext) -> void;
}