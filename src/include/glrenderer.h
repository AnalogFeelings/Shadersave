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