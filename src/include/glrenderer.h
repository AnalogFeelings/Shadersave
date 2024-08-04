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
#include <settings.h>
#include <GL/wglew.h>

typedef struct
{
	FLOAT Time;
	FLOAT DeltaTime;
	FLOAT FrameRate;

	INT Year;
	INT Month;
	INT Day;
	INT Seconds;
} UNIFORMS, *PUNIFORMS;

class GLRenderer
{
public:
	std::shared_ptr<Shader> QuadShader;
	std::shared_ptr<Shader> BufferAShader;
	std::shared_ptr<Shader> BufferBShader;
	std::shared_ptr<Shader> BufferCShader;
	std::shared_ptr<Shader> BufferDShader;

	auto InitContext(HWND hWnd, HDC& deviceContext, HGLRC& glRenderContext) -> BOOL;
	auto InitRenderer(INT viewportWidth, INT viewportHeight, CONST SETTINGS& settings) -> BOOL;

	auto DoRender(HDC deviceContext) -> VOID;
	~GLRenderer();

private:
	INT ViewportWidth = 0;
	INT ViewportHeight = 0;
	INT FrameCount = 0;

	UINT QuadVao = 0;
	UINT QuadVbo = 0;
	UINT QuadEbo = 0;

	UINT BufferAFramebuffer = 0;
	UINT BufferATexture = 0;

	UINT BufferBFramebuffer = 0;
	UINT BufferBTexture = 0;

	UINT BufferCFramebuffer = 0;
	UINT BufferCTexture = 0;

	UINT BufferDFramebuffer = 0;
	UINT BufferDTexture = 0;

	ULONG64 ProgramStart = 0;
	ULONG64 ProgramNow = 0;
	ULONG64 ProgramDelta = 0;

	UINT Channel0Texture = 0;
	UINT Channel1Texture = 0;
	UINT Channel2Texture = 0;
	UINT Channel3Texture = 0;

	auto SetUniformValues(std::shared_ptr<Shader> target, PUNIFORMS uniforms) -> VOID;
	auto LoadFileFromResource(INT resourceId, UINT& size, PCSTR& data) -> BOOL;
	auto GuaranteeNullTermination(UINT size, CONST PCSTR& data) -> std::string;
	auto LoadFileFromDisk(CONST std::string& filename) -> std::string;
	auto CreateShader(std::shared_ptr<Shader>& target, CONST std::string& vertexSource, std::string& fragmentSource) -> BOOL;
	auto GetUnixTimeInMs() -> ULONG64;
};
