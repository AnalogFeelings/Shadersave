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
#include <format>
#include <memory>
#include <string>
#include <shader.h>
#include <resources.h>
#include <chrono>

#define TIMEPOINT std::chrono::time_point<std::chrono::high_resolution_clock>

class GLRenderer
{
public:
	std::shared_ptr<Shader> QuadShader;

	auto InitContext(HWND hWnd, HDC& deviceContext, HGLRC& glRenderContext) -> BOOL;
	auto InitRenderer(INT viewportWidth, INT viewportHeight) -> BOOL;

	auto DoRender(HDC deviceContext) -> VOID;

	auto CloseRenderer(HWND hWnd, HDC deviceContext, HGLRC glRenderContext) -> VOID;
private:
	INT ViewportWidth = 0;
	INT ViewportHeight = 0;

	UINT QuadVao, QuadVbo, QuadEbo;

	TIMEPOINT ProgramStart;
	TIMEPOINT ProgramNow;
	
	CONST FLOAT QuadVertices[12] =
	{
		-1.0f, -1.0f, -0.0f,
		1.0f,  1.0f, -0.0f,
		-1.0f,  1.0f, -0.0f,
		1.0f, -1.0f, -0.0f
	};
	CONST UINT QuadIndices[6] =
	{
		0, 1, 2,
		0, 3, 1
	};

	auto LoadFileFromResource(INT resourceId, UINT& size, PCSTR& data) -> BOOL;
	auto GuaranteeNull(UINT size, PCSTR& data) -> std::string;
};