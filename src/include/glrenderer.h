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
#include <defines.h>
#include <iostream>
#include <fstream>

#include "settings.h"

class GLRenderer
{
public:
	std::shared_ptr<Shader> QuadShader;

	auto InitContext(HWND hWnd, HDC& deviceContext, HGLRC& glRenderContext) -> BOOL;
	auto InitRenderer(INT viewportWidth, INT viewportHeight, SETTINGS settings) -> BOOL;

	auto DoRender(HDC deviceContext) -> VOID;

	auto CloseRenderer(HWND hWnd, HDC deviceContext, HGLRC glRenderContext) -> VOID;

private:
	INT ViewportWidth = 0;
	INT ViewportHeight = 0;
	INT FrameCount = 0;

	UINT QuadVao = 0;
	UINT QuadVbo = 0;
	UINT QuadEbo = 0;

	ULONG64 ProgramStart = 0;
	ULONG64 ProgramNow = 0;

	auto LoadFileFromResource(INT resourceId, UINT& size, PCSTR& data) -> BOOL;
	auto GuaranteeNull(UINT size, PCSTR& data) -> std::string;
	auto GetUnixTimeInMs() -> ULONG64;
};
