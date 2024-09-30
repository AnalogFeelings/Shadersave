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

#include <Classes/Shader.h>
#include <Classes/Buffer.h>
#include <Resources.h>
#include <Renderer.h>
#include <Globals.h>

#include <memory>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <GL/wglew.h>
#include <format>

#define SETUP_BUFFER_BINDINGS(Buffer) \
	unsigned int channel0 = 0; \
	unsigned int channel1 = 0; \
	unsigned int channel2 = 0; \
	unsigned int channel3 = 0; \
	if (settings.Buffer##Channel0 == BUFFER_A) \
		channel0 = BufferATexture; \
	if (settings.Buffer##Channel0 == BUFFER_B) \
		channel0 = BufferBTexture; \
	if (settings.Buffer##Channel0 == BUFFER_C) \
		channel0 = BufferCTexture; \
	if (settings.Buffer##Channel0 == BUFFER_D) \
		channel0 = BufferDTexture; \
	 \
	if (settings.Buffer##Channel1 == BUFFER_A) \
		channel1 = BufferATexture; \
	if (settings.Buffer##Channel1 == BUFFER_B) \
		channel1 = BufferBTexture; \
	if (settings.Buffer##Channel1 == BUFFER_C) \
		channel1 = BufferCTexture; \
	if (settings.Buffer##Channel1 == BUFFER_D) \
		channel1 = BufferDTexture; \
	 \
	if (settings.Buffer##Channel2 == BUFFER_A) \
		channel2 = BufferATexture; \
	if (settings.Buffer##Channel2 == BUFFER_B) \
		channel2 = BufferBTexture; \
	if (settings.Buffer##Channel2 == BUFFER_C) \
		channel2 = BufferCTexture; \
	if (settings.Buffer##Channel2 == BUFFER_D) \
		channel2 = BufferDTexture; \
	 \
	if (settings.Buffer##Channel3 == BUFFER_A) \
		channel3 = BufferATexture; \
	if (settings.Buffer##Channel3 == BUFFER_B) \
		channel3 = BufferBTexture; \
	if (settings.Buffer##Channel3 == BUFFER_C) \
		channel3 = BufferCTexture; \
	if (settings.Buffer##Channel3 == BUFFER_D) \
		channel3 = BufferDTexture

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

unsigned int BufferATexture;
unsigned int BufferBTexture;
unsigned int BufferCTexture;
unsigned int BufferDTexture;

int ViewportWidth = 0;
int ViewportHeight = 0;
int FrameCount = 0;

unsigned int QuadVao = 0;
unsigned int QuadVbo = 0;
unsigned int QuadEbo = 0;

unsigned long ProgramStart = 0;
unsigned long ProgramNow = 0;
unsigned long ProgramDelta = 0;

unsigned int QuadChannel0 = 0;
unsigned int QuadChannel1 = 0;
unsigned int QuadChannel2 = 0;
unsigned int QuadChannel3 = 0;

std::unique_ptr<Shader> QuadShader;
std::unique_ptr<Buffer> BufferA;
std::unique_ptr<Buffer> BufferB;
std::unique_ptr<Buffer> BufferC;
std::unique_ptr<Buffer> BufferD;

auto LoadFileFromResource(int resourceId, unsigned int& size, const char* data) -> bool;
auto GuaranteeNullTermination(unsigned int size, const char* data) -> std::string;
auto LoadFileFromDisk(const std::string& filename) -> std::string;
auto CreateShader(const std::unique_ptr<Shader>& target, const std::string& vertexSource, std::string& fragmentSource) -> bool;
auto GetUnixTimeInMs() -> unsigned long;

auto Renderer::InitContext(HWND hWnd, HDC& deviceContext, HGLRC& glRenderContext) -> bool
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

	// Needed to make a modern context.
	HGLRC fakeContext = wglCreateContext(deviceContext);
	wglMakeCurrent(deviceContext, fakeContext);

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		NULL
	};
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribs = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));

	glRenderContext = wglCreateContextAttribs(deviceContext, nullptr, attribs);

	wglDeleteContext(fakeContext);
	wglMakeCurrent(deviceContext, glRenderContext);

	GLenum glError = glewInit();
	if (glError != GLEW_OK)
	{
		GLenum error = glGetError();
		char buffer[GLEW_ERROR_SIZE];

		std::snprintf(buffer, GLEW_ERROR_SIZE, "%s", glewGetErrorString(error));
		Globals::LastError = buffer;

		return false;
	}

	return true;
}

auto Renderer::InitRenderer(int viewportWidth, int viewportHeight, const RenderSettings& settings) -> bool
{
	ViewportWidth = viewportWidth;
	ViewportHeight = viewportHeight;

	glViewport(0, 0, ViewportWidth, ViewportHeight);

	glGenVertexArrays(1, &QuadVao);
	glGenBuffers(1, &QuadVbo);
	glGenBuffers(1, &QuadEbo);
	glBindVertexArray(QuadVao);

	glBindBuffer(GL_ARRAY_BUFFER, QuadVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTICES), QUAD_VERTICES, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, QuadEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_INDICES), QUAD_INDICES, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Initialize buffer textures.
	glGenTextures(1, &BufferATexture);

	glBindTexture(GL_TEXTURE_2D, BufferATexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportWidth, viewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &BufferBTexture);

	glBindTexture(GL_TEXTURE_2D, BufferBTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportWidth, viewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &BufferCTexture);

	glBindTexture(GL_TEXTURE_2D, BufferCTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportWidth, viewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &BufferDTexture);

	glBindTexture(GL_TEXTURE_2D, BufferDTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportWidth, viewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int vertexSize = 0;
	const char* vertexData = nullptr;

	// Load vertex shader text.
	bool vertexResult = LoadFileFromResource(IDR_VERTEXSHADER, vertexSize, vertexData);
	if (!vertexResult)
		return false;

	std::string vertexSource = GuaranteeNullTermination(vertexSize, vertexData);
	std::string fragmentSource;

	if (settings.MainPath.empty())
	{
		unsigned int fragmentSize = 0;
		const char* fragmentData = nullptr;

		// Load fragment shader text.
		bool fragmentResult = LoadFileFromResource(IDR_FRAGMENTSHADER, fragmentSize, fragmentData);
		if (!fragmentResult)
			return false;

		fragmentSource = GuaranteeNullTermination(fragmentSize, fragmentData);
	}
	else
	{
		fragmentSource = LoadFileFromDisk(settings.MainPath);
	}

	// Actually load the shaders and compile them.
	QuadShader = std::make_unique<Shader>();
	bool quadResult = CreateShader(QuadShader, vertexSource, fragmentSource);
	if (!quadResult)
		return false;

	// Initialize main quad shader bindings.
	{
		SETUP_BUFFER_BINDINGS(Main);

		QuadChannel0 = channel0;
		QuadChannel1 = channel1;
		QuadChannel2 = channel2;
		QuadChannel3 = channel3;
	}

	// Create buffers.
	if (!settings.BufferAPath.empty())
	{
		BufferA = std::make_unique<Buffer>();

		std::string bufferSource = LoadFileFromDisk(settings.BufferAPath);
		std::unique_ptr<Shader> shader = std::make_unique<Shader>();

		bool shaderResult = CreateShader(shader, vertexSource, bufferSource);
		if (!shaderResult)
			return false;

		bool bufferResult = BufferA->SetupBuffer(&BufferATexture, ViewportWidth, ViewportHeight, BUFFERA_START, shader);
		if (!bufferResult)
			return false;

		// Let's initialize the channels.
		SETUP_BUFFER_BINDINGS(BufferA);

		BufferA->SetupChannels(channel0, channel1, channel2, channel3);
	}
	if (!settings.BufferBPath.empty())
	{
		BufferB = std::make_unique<Buffer>();

		std::string bufferSource = LoadFileFromDisk(settings.BufferBPath);
		std::unique_ptr<Shader> shader = std::make_unique<Shader>();

		bool shaderResult = CreateShader(shader, vertexSource, bufferSource);
		if (!shaderResult)
			return false;

		bool bufferResult = BufferB->SetupBuffer(&BufferBTexture, ViewportWidth, ViewportHeight, BUFFERB_START, shader);
		if (!bufferResult)
			return false;

		// Let's initialize the channels.
		SETUP_BUFFER_BINDINGS(BufferB);

		BufferB->SetupChannels(channel0, channel1, channel2, channel3);
	}
	if (!settings.BufferCPath.empty())
	{
		BufferC = std::make_unique<Buffer>();

		std::string bufferSource = LoadFileFromDisk(settings.BufferCPath);
		std::unique_ptr<Shader> shader = std::make_unique<Shader>();

		bool shaderResult = CreateShader(shader, vertexSource, bufferSource);
		if (!shaderResult)
			return false;

		bool bufferResult = BufferC->SetupBuffer(&BufferCTexture, ViewportWidth, ViewportHeight, BUFFERC_START, shader);
		if (!bufferResult)
			return false;

		// Let's initialize the channels.
		SETUP_BUFFER_BINDINGS(BufferC);

		BufferC->SetupChannels(channel0, channel1, channel2, channel3);
	}
	if (!settings.BufferDPath.empty())
	{
		BufferD = std::make_unique<Buffer>();

		std::string bufferSource = LoadFileFromDisk(settings.BufferDPath);
		std::unique_ptr<Shader> shader = std::make_unique<Shader>();

		bool shaderResult = CreateShader(shader, vertexSource, bufferSource);
		if (!shaderResult)
			return false;

		bool bufferResult = BufferD->SetupBuffer(&BufferDTexture, ViewportWidth, ViewportHeight, BUFFERD_START, shader);
		if (!bufferResult)
			return false;

		// Let's initialize the channels.
		SETUP_BUFFER_BINDINGS(BufferD);

		BufferD->SetupChannels(channel0, channel1, channel2, channel3);
	}

	// Set up startup time.
	ProgramStart = GetUnixTimeInMs();

	return TRUE;
}

auto Renderer::DoRender(HDC deviceContext) -> VOID
{
	ProgramNow = GetUnixTimeInMs();

	std::time_t currentCTime = std::time(nullptr);
	std::tm* detailedTime = std::localtime(&currentCTime);

	Uniforms uniforms =
	{
		.ViewportWidth = ViewportWidth,
		.ViewportHeight = ViewportHeight,

		.Time = (ProgramNow - ProgramStart) / 1000.0f,
		.DeltaTime = ProgramDelta / 1000.0f,
		.FrameRate = 1000.0f / ProgramDelta,

		.Year = detailedTime->tm_year + 1900,
		.Month = detailedTime->tm_mon + 1,
		.Day = detailedTime->tm_mday,
		.Seconds = (detailedTime->tm_hour * 3600) + (detailedTime->tm_min * 60) + detailedTime->tm_sec,

		.FrameCount = FrameCount
	};

	if (BufferA)
	{
		BufferA->SetupRender(uniforms);

		glBindVertexArray(QuadVao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}
	if (BufferB)
	{
		BufferB->SetupRender(uniforms);

		glBindVertexArray(QuadVao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}
	if (BufferC)
	{
		BufferC->SetupRender(uniforms);

		glBindVertexArray(QuadVao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}
	if (BufferD)
	{
		BufferD->SetupRender(uniforms);

		glBindVertexArray(QuadVao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, QuadChannel0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, QuadChannel1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, QuadChannel2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, QuadChannel3);

	QuadShader->SetVector3Uniform("iResolution", uniforms.ViewportWidth, uniforms.ViewportHeight, 0);
	QuadShader->SetFloatUniform("iTime", uniforms.Time);
	QuadShader->SetFloatUniform("iTimeDelta", uniforms.DeltaTime);
	QuadShader->SetFloatUniform("iFrameRate", uniforms.FrameRate);
	QuadShader->SetIntUniform("iFrame", FrameCount);

	// Set texture units.
	QuadShader->SetIntUniform("iChannel0", 0);
	QuadShader->SetIntUniform("iChannel1", 1);
	QuadShader->SetIntUniform("iChannel2", 2);
	QuadShader->SetIntUniform("iChannel3", 3);

	QuadShader->SetVector4Uniform("iDate", uniforms.Year, uniforms.Month, uniforms.Day, uniforms.Seconds);
	QuadShader->UseShader();

	glBindVertexArray(QuadVao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	::SwapBuffers(deviceContext);

	unsigned long currentTime = GetUnixTimeInMs();
	ProgramDelta = currentTime - ProgramNow;

	FrameCount++;
}

auto Renderer::UninitializeRenderer() -> void
{
	glDeleteVertexArrays(1, &QuadVao);
	glDeleteBuffers(1, &QuadVbo);
	glDeleteBuffers(1, &QuadEbo);

	glDeleteTextures(1, &BufferATexture);
	glDeleteTextures(1, &BufferBTexture);
	glDeleteTextures(1, &BufferCTexture);
	glDeleteTextures(1, &BufferDTexture);

	glDeleteTextures(1, &QuadChannel0);
	glDeleteTextures(1, &QuadChannel1);
	glDeleteTextures(1, &QuadChannel2);
	glDeleteTextures(1, &QuadChannel3);

	BufferA.reset();
	BufferB.reset();
	BufferC.reset();
	BufferD.reset();

	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(Globals::GlRenderContext);
	::ReleaseDC(Globals::MainWindow, Globals::DeviceContext);
}

auto LoadFileFromResource(int resourceId, unsigned int& size, const char* data) -> bool
{
	HMODULE moduleHandle = ::GetModuleHandle(nullptr);
	HRSRC resourceHandle = ::FindResource(moduleHandle, MAKEINTRESOURCE(resourceId), "TEXT");

	if (!resourceHandle)
		return false;

	HGLOBAL resourceData = ::LoadResource(moduleHandle, resourceHandle);

	if (!resourceData)
		return false;

	size = ::SizeofResource(moduleHandle, resourceHandle);
	data = static_cast<PCSTR>(::LockResource(resourceData));

	return true;
}

auto GuaranteeNullTermination(unsigned int size, const char* data) -> std::string
{
	std::unique_ptr<CHAR[]> buffer = std::make_unique<CHAR[]>(size + 1);
	std::memcpy(buffer.get(), data, size);

	buffer.get()[size] = NULL;

	return std::string(buffer.get());
}

auto LoadFileFromDisk(const std::string& filename) -> std::string
{
	std::ifstream fileStream(filename);
	std::stringstream stringStream;
	stringStream << fileStream.rdbuf();

	std::string text = stringStream.str();

	fileStream.close();

	return text;
}

auto CreateShader(const std::unique_ptr<Shader>& target, const std::string& vertexSource, std::string& fragmentSource) -> bool
{
	bool shaderResult = target->LoadShader(vertexSource);
	if (!shaderResult)
		return false;

	bool shadertoyResult = target->LoadShadertoyShader(fragmentSource);
	if (!shadertoyResult)
		return false;

	bool compileResult = target->CreateProgram();
	if (!compileResult)
		return false;

	return true;
}

auto GetUnixTimeInMs() -> unsigned long
{
	Timepoint currentPoint = std::chrono::system_clock::now();

	return std::chrono::duration_cast<std::chrono::milliseconds>(currentPoint.time_since_epoch()).count();
}