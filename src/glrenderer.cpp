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

#include <glrenderer.h>

constexpr FLOAT QUAD_VERTICES[12] =
{
	-1.0f, -1.0f, -0.0f,
	+1.0f, +1.0f, -0.0f,
	-1.0f, +1.0f, -0.0f,
	+1.0f, -1.0f, -0.0f
};
constexpr UINT QUAD_INDICES[6] =
{
	0, 1, 2,
	0, 3, 1
};

auto GLRenderer::InitContext(HWND hWnd, HDC& deviceContext, HGLRC& glRenderContext) -> BOOL
{
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor =
	{
		.nSize = sizeof(PIXELFORMATDESCRIPTOR),
		.nVersion = 1,
		.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		.iPixelType = PFD_TYPE_RGBA,
		.cColorBits = 24
	};

	deviceContext = ::GetDC(hWnd);

	int chosenFormat = ::ChoosePixelFormat(deviceContext, &pixelFormatDescriptor);
	::SetPixelFormat(deviceContext, chosenFormat, &pixelFormatDescriptor);

	glRenderContext = wglCreateContext(deviceContext);
	wglMakeCurrent(deviceContext, glRenderContext);

	GLenum glError = glewInit();
	if (glError != GLEW_OK)
	{
		return FALSE;
	}

	return TRUE;
}

auto GLRenderer::InitRenderer(INT viewportWidth, INT viewportHeight, SETTINGS settings) -> BOOL
{
	this->ViewportWidth = viewportWidth;
	this->ViewportHeight = viewportHeight;

	glViewport(0, 0, this->ViewportWidth, this->ViewportHeight);

	glGenVertexArrays(1, &this->QuadVao);
	glGenBuffers(1, &this->QuadVbo);
	glGenBuffers(1, &this->QuadEbo);

	glBindVertexArray(this->QuadVao);

	glBindBuffer(GL_ARRAY_BUFFER, this->QuadVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTICES), QUAD_VERTICES, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->QuadEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_INDICES), QUAD_INDICES, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(FLOAT), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	UINT vertexSize;
	PCSTR vertexData;

	// Load vertex shader text.
	BOOL vertexResult = this->LoadFileFromResource(IDR_VERTEXSHADER, vertexSize, vertexData);
	if (!vertexResult)
		return FALSE;

	std::string vertexSource = this->GuaranteeNull(vertexSize, vertexData);
	std::string fragmentSource;

	if(settings.ShaderPath.empty())
	{
		UINT fragmentSize;
		PCSTR fragmentData;

		// Load fragment shader text.
		BOOL fragmentResult = this->LoadFileFromResource(IDR_FRAGMENTSHADER, fragmentSize, fragmentData);
		if (!fragmentResult)
			return FALSE;

		fragmentSource = this->GuaranteeNull(fragmentSize, fragmentData);
	}
	else
	{
		std::ifstream fileStream(settings.ShaderPath);
		std::stringstream stringStream;
		stringStream << fileStream.rdbuf();

		fragmentSource = stringStream.str();

		fileStream.close();
	}

	// Actually load the shaders and compile them.
	this->QuadShader = std::make_shared<Shader>();
	BOOL shaderResult = this->QuadShader->LoadShader(vertexSource);
	if (!shaderResult)
		return FALSE;

	BOOL shadertoyResult = this->QuadShader->LoadShadertoyShader(fragmentSource);
	if (!shadertoyResult)
		return FALSE;

	BOOL compileResult = this->QuadShader->CreateProgram();
	if (!compileResult)
		return FALSE;

	// Set up startup time.
	this->ProgramStart = std::chrono::high_resolution_clock::now();

	return TRUE;
}

VOID GLRenderer::DoRender(HDC deviceContext)
{
	this->ProgramNow = std::chrono::high_resolution_clock::now();

	// Looks dirty, I know.
	FLOAT elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(this->ProgramNow - this->ProgramStart).count() / 1000.0;

	this->QuadShader->SetVector3Uniform("iResolution", this->ViewportWidth, this->ViewportHeight, 0);
	this->QuadShader->SetFloatUniform("iTime", elapsedTime);
	this->QuadShader->SetIntUniform("iFrame", this->FrameCount);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	this->QuadShader->UseShader();
	glBindVertexArray(this->QuadVao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	::SwapBuffers(deviceContext);

	this->FrameCount++;
}

VOID GLRenderer::CloseRenderer(HWND hWnd, HDC deviceContext, HGLRC glRenderContext)
{
	glDeleteVertexArrays(1, &this->QuadVao);
	glDeleteBuffers(1, &this->QuadVbo);
	glDeleteBuffers(1, &this->QuadEbo);

	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(glRenderContext);
	::ReleaseDC(hWnd, deviceContext);
}

auto GLRenderer::LoadFileFromResource(INT resourceId, UINT& size, PCSTR& data) -> BOOL
{
	HMODULE moduleHandle = ::GetModuleHandle(nullptr);
	HRSRC resourceHandle = ::FindResource(moduleHandle, MAKEINTRESOURCE(resourceId), "TEXT");

	if (!resourceHandle)
		return FALSE;

	HGLOBAL resourceData = ::LoadResource(moduleHandle, resourceHandle);

	if (!resourceData)
		return FALSE;

	size = ::SizeofResource(moduleHandle, resourceHandle);
	data = static_cast<PCSTR>(::LockResource(resourceData));

	return TRUE;
}

auto GLRenderer::GuaranteeNull(UINT size, PCSTR& data) -> std::string
{
	std::unique_ptr<CHAR[]> buffer = std::make_unique<CHAR[]>(size + 1);
	std::memcpy(buffer.get(), data, size);

	buffer.get()[size] = NULL;

	return std::string(buffer.get());
}
