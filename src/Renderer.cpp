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

#include <Classes/Shader.h>
#include <Classes/Buffer.h>
#include <Resources.h>
#include <Renderer.h>
#include <Globals.h>

#include <filesystem>
#include <memory>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <format>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int ViewportWidth = 0;
int ViewportHeight = 0;
int FrameCount = 0;

unsigned int QuadVao = 0;
unsigned int QuadVbo = 0;
unsigned int QuadEbo = 0;

unsigned long ProgramStart = 0;
unsigned long ProgramNow = 0;
unsigned long ProgramDelta = 0;

std::unique_ptr<Shader> QuadShader;
unsigned int QuadChannels[CHANNEL_COUNT];
Vector3 QuadChannelResolutions[CHANNEL_COUNT];

std::unique_ptr<Buffer> Buffers[BUFFER_COUNT];
unsigned int BufferTextures[BUFFER_COUNT];

auto LoadTexture(const std::string& filename, unsigned int& texture, int& width, int& height) -> bool;
auto LoadFileFromResource(int resourceId, unsigned int& size, const char*& data) -> bool;
auto GuaranteeNullTermination(unsigned int size, const char* data) -> std::string;
auto LoadFileFromDisk(const std::string& filename) -> std::string;
auto CreateShader(const std::unique_ptr<Shader>& target, const std::string& vertexSource, std::string& fragmentSource, const std::string& commonSource) -> bool;
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
	for (unsigned int& bufferTexture : BufferTextures)
	{
		glGenTextures(1, &bufferTexture);

		glBindTexture(GL_TEXTURE_2D, bufferTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewportWidth, viewportHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

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

	std::string commonSource;

	if (!settings.CommonPath.empty())
		commonSource = LoadFileFromDisk(settings.CommonPath);

	// Actually load the shaders and compile them.
	QuadShader = std::make_unique<Shader>();
	bool quadResult = CreateShader(QuadShader, vertexSource, fragmentSource, commonSource);
	if (!quadResult)
		return false;

	// Initialize main quad shader bindings.
	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		const std::string mainChannel = settings.MainChannels[i];

		if (mainChannel == BUFFER_A)
			QuadChannels[i] = BufferTextures[0];
		else if (mainChannel == BUFFER_B)
			QuadChannels[i] = BufferTextures[1];
		else if (mainChannel == BUFFER_C)
			QuadChannels[i] = BufferTextures[2];
		else if (mainChannel == BUFFER_D)
			QuadChannels[i] = BufferTextures[3];
		else if (std::filesystem::is_regular_file(mainChannel))
		{
			int width, height;
			bool result = LoadTexture(mainChannel, QuadChannels[i], width, height);

			if (!result)
				return false;

			QuadChannelResolutions[i] = Vector3(width, height, 0);
		}

		if (Globals::ValidBindings.contains(mainChannel))
			QuadChannelResolutions[i] = Vector3(viewportWidth, viewportHeight, 0);
	}

	// Create buffers.
	unsigned int channels[CHANNEL_COUNT];
	Vector3 channelResolutions[CHANNEL_COUNT];

	if (!settings.BufferAPath.empty())
	{
		Buffers[0] = std::make_unique<Buffer>();

		std::string bufferSource = LoadFileFromDisk(settings.BufferAPath);
		std::unique_ptr<Shader> shader = std::make_unique<Shader>();

		bool shaderResult = CreateShader(shader, vertexSource, bufferSource, commonSource);
		if (!shaderResult)
			return false;

		bool bufferResult = Buffers[0]->SetupBuffer(&BufferTextures[0], ViewportWidth, ViewportHeight, BUFFERA_START, shader);
		if (!bufferResult)
			return false;

		// Let's initialize the channels.
		for (int i = 0; i < CHANNEL_COUNT; i++)
		{
			const std::string bufferAChannel = settings.BufferAChannels[i];

			if (bufferAChannel == BUFFER_A)
				channels[i] = BufferTextures[0];
			if (bufferAChannel == BUFFER_B)
				channels[i] = BufferTextures[1];
			if (bufferAChannel == BUFFER_C)
				channels[i] = BufferTextures[2];
			if (bufferAChannel == BUFFER_D)
				channels[i] = BufferTextures[3];
			else if (std::filesystem::is_regular_file(bufferAChannel))
			{
				int width, height;
				bool result = LoadTexture(bufferAChannel, channels[i], width, height);

				if (!result)
					return false;

				channelResolutions[i] = Vector3(width, height, 0);
			}

			if (Globals::ValidBindings.contains(bufferAChannel))
				channelResolutions[i] = Vector3(viewportWidth, viewportHeight, 0);
		}

		Buffers[0]->SetupChannels(channels, channelResolutions);
	}
	if (!settings.BufferBPath.empty())
	{
		Buffers[1] = std::make_unique<Buffer>();

		std::string bufferSource = LoadFileFromDisk(settings.BufferBPath);
		std::unique_ptr<Shader> shader = std::make_unique<Shader>();

		bool shaderResult = CreateShader(shader, vertexSource, bufferSource, commonSource);
		if (!shaderResult)
			return false;

		bool bufferResult = Buffers[1]->SetupBuffer(&BufferTextures[1], ViewportWidth, ViewportHeight, BUFFERB_START, shader);
		if (!bufferResult)
			return false;

		// Let's initialize the channels.
		for (int i = 0; i < CHANNEL_COUNT; i++)
		{
			const std::string bufferBChannel = settings.BufferBChannels[i];

			if (bufferBChannel == BUFFER_A)
				channels[i] = BufferTextures[0];
			if (bufferBChannel == BUFFER_B)
				channels[i] = BufferTextures[1];
			if (bufferBChannel == BUFFER_C)
				channels[i] = BufferTextures[2];
			if (bufferBChannel == BUFFER_D)
				channels[i] = BufferTextures[3];
			else if (std::filesystem::is_regular_file(bufferBChannel))
			{
				int width, height;
				bool result = LoadTexture(bufferBChannel, channels[i], width, height);

				if (!result)
					return false;

				channelResolutions[i] = Vector3(width, height, 0);
			}

			if (Globals::ValidBindings.contains(bufferBChannel))
				channelResolutions[i] = Vector3(viewportWidth, viewportHeight, 0);
		}

		Buffers[1]->SetupChannels(channels, channelResolutions);
	}
	if (!settings.BufferCPath.empty())
	{
		Buffers[2] = std::make_unique<Buffer>();

		std::string bufferSource = LoadFileFromDisk(settings.BufferCPath);
		std::unique_ptr<Shader> shader = std::make_unique<Shader>();

		bool shaderResult = CreateShader(shader, vertexSource, bufferSource, commonSource);
		if (!shaderResult)
			return false;

		bool bufferResult = Buffers[2]->SetupBuffer(&BufferTextures[2], ViewportWidth, ViewportHeight, BUFFERC_START, shader);
		if (!bufferResult)
			return false;

		// Let's initialize the channels.
		for (int i = 0; i < CHANNEL_COUNT; i++)
		{
			const std::string bufferCChannel = settings.BufferCChannels[i];

			if (bufferCChannel == BUFFER_A)
				channels[i] = BufferTextures[0];
			if (bufferCChannel == BUFFER_B)
				channels[i] = BufferTextures[1];
			if (bufferCChannel == BUFFER_C)
				channels[i] = BufferTextures[2];
			if (bufferCChannel == BUFFER_D)
				channels[i] = BufferTextures[3];
			else if (std::filesystem::is_regular_file(bufferCChannel))
			{
				int width, height;
				bool result = LoadTexture(bufferCChannel, channels[i], width, height);

				if (!result)
					return false;

				channelResolutions[i] = Vector3(width, height, 0);
			}

			if (Globals::ValidBindings.contains(bufferCChannel))
				channelResolutions[i] = Vector3(viewportWidth, viewportHeight, 0);
		}

		Buffers[2]->SetupChannels(channels, channelResolutions);
	}
	if (!settings.BufferDPath.empty())
	{
		Buffers[3] = std::make_unique<Buffer>();

		std::string bufferSource = LoadFileFromDisk(settings.BufferDPath);
		std::unique_ptr<Shader> shader = std::make_unique<Shader>();

		bool shaderResult = CreateShader(shader, vertexSource, bufferSource, commonSource);
		if (!shaderResult)
			return false;

		bool bufferResult = Buffers[3]->SetupBuffer(&BufferTextures[3], ViewportWidth, ViewportHeight, BUFFERD_START, shader);
		if (!bufferResult)
			return false;

		// Let's initialize the channels.
		for (int i = 0; i < CHANNEL_COUNT; i++)
		{
			const std::string bufferDChannel = settings.BufferDChannels[i];

			if (bufferDChannel == BUFFER_A)
				channels[i] = BufferTextures[0];
			if (bufferDChannel == BUFFER_B)
				channels[i] = BufferTextures[1];
			if (bufferDChannel == BUFFER_C)
				channels[i] = BufferTextures[2];
			if (bufferDChannel == BUFFER_D)
				channels[i] = BufferTextures[3];
			else if (std::filesystem::is_regular_file(bufferDChannel))
			{
				int width, height;
				bool result = LoadTexture(bufferDChannel, channels[i], width, height);

				if (!result)
					return false;

				channelResolutions[i] = Vector3(width, height, 0);
			}

			if (Globals::ValidBindings.contains(bufferDChannel))
				channelResolutions[i] = Vector3(viewportWidth, viewportHeight, 0);
		}

		Buffers[3]->SetupChannels(channels, channelResolutions);
	}

	// Set some uniforms ahead of time for performance.
	QuadShader->SetVector3Uniform("iResolution", viewportWidth, viewportHeight, 0);
	QuadShader->SetVector3ArrayUniform("iChannelResolution", QuadChannelResolutions, CHANNEL_COUNT);

	QuadShader->SetIntUniform("iChannel0", 0);
	QuadShader->SetIntUniform("iChannel1", 1);
	QuadShader->SetIntUniform("iChannel2", 2);
	QuadShader->SetIntUniform("iChannel3", 3);

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
		.Time = (ProgramNow - ProgramStart) / 1000.0f,
		.DeltaTime = ProgramDelta / 1000.0f,
		.FrameRate = 1000.0f / ProgramDelta,

		.Year = detailedTime->tm_year + 1900,
		.Month = detailedTime->tm_mon + 1,
		.Day = detailedTime->tm_mday,
		.Seconds = (detailedTime->tm_hour * 3600) + (detailedTime->tm_min * 60) + detailedTime->tm_sec,

		.FrameCount = FrameCount
	};

	glBindVertexArray(QuadVao);
	for (std::unique_ptr<Buffer>& buffer : Buffers)
	{
		if (buffer)
		{
			buffer->SetupRender(uniforms);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		}
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, QuadChannels[i]);
	}

	QuadShader->SetFloatUniform("iTime", uniforms.Time);
	QuadShader->SetFloatUniform("iTimeDelta", uniforms.DeltaTime);
	QuadShader->SetFloatUniform("iFrameRate", uniforms.FrameRate);
	QuadShader->SetIntUniform("iFrame", FrameCount);

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

	for (unsigned int& bufferTexture : BufferTextures)
	{
		glDeleteTextures(1, &bufferTexture);
	}

	for (unsigned int& quadChannel : QuadChannels)
	{
		glDeleteTextures(1, &quadChannel);
	}

	for (std::unique_ptr<Buffer>& buffer : Buffers)
	{
		buffer.reset();
	}

	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(Globals::GlRenderContext);
	::ReleaseDC(Globals::MainWindow, Globals::DeviceContext);
}

auto LoadTexture(const std::string& filename, unsigned int& texture, int& width, int& height) -> bool
{
	stbi_set_flip_vertically_on_load(true);

	int numberOfChannels;
	unsigned char* image = stbi_load(filename.c_str(), &width, &height, &numberOfChannels, STBI_rgb_alpha);

	if (!image)
	{
		Globals::LastError = "Could not load image file " + filename + ".";
		return false;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(image);

	return true;
}

auto LoadFileFromResource(int resourceId, unsigned int& size, const char*& data) -> bool
{
	HMODULE moduleHandle = ::GetModuleHandle(nullptr);
	HRSRC resourceHandle = ::FindResource(moduleHandle, MAKEINTRESOURCE(resourceId), "TEXT");

	if (!resourceHandle)
		return false;

	HGLOBAL resourceData = ::LoadResource(moduleHandle, resourceHandle);

	if (!resourceData)
		return false;

	size = ::SizeofResource(moduleHandle, resourceHandle);
	data = static_cast<const char*>(::LockResource(resourceData));

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

auto CreateShader(const std::unique_ptr<Shader>& target, const std::string& vertexSource, std::string& fragmentSource, const std::string& commonSource) -> bool
{
	bool shaderResult = target->LoadShader(vertexSource);
	if (!shaderResult)
		return false;

	bool shadertoyResult = target->LoadShadertoyShader(fragmentSource, commonSource);
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