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

#include <Classes/FrameLimiter.h>
#include <Classes/Shader.h>
#include <Classes/Buffer.h>
#include <Resources.h>
#include <Renderer.h>
#include <Globals.h>

#include <filesystem>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GL/wglew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int32_t ViewportWidth = 0;
int32_t ViewportHeight = 0;
int32_t FrameCount = 0;

uint32_t QuadVao = 0;
uint32_t QuadVbo = 0;
uint32_t QuadEbo = 0;

uint32_t ProgramStart = 0;
uint32_t FrameStartTime = 0;
uint32_t ProgramDelta = 0;

std::unique_ptr<Shader> QuadShader;
uint32_t QuadChannels[CHANNEL_COUNT];
Vector3 QuadChannelResolutions[CHANNEL_COUNT];

std::unique_ptr<Buffer> Buffers[BUFFER_COUNT];
uint32_t BufferTextures[BUFFER_COUNT];

Uniforms ShaderUniforms;

FrameLimiter frameLimiter;

auto LoadTexture(const std::string& filename, uint32_t& texture, int32_t& width, int32_t& height) -> bool;
auto LoadFileFromResource(int32_t resourceId, uint32_t& size, const char*& data) -> bool;
auto GuaranteeNullTermination(uint32_t size, const char* data) -> std::string;
auto LoadFileFromDisk(const std::string& filename) -> std::string;
auto CreateShader(const std::unique_ptr<Shader>& target, const std::string& vertexSource, std::string& fragmentSource, const std::string& commonSource) -> bool;
auto GetUnixTimeInMs() -> uint32_t;

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

	int32_t chosenFormat = ::ChoosePixelFormat(deviceContext, &pixelFormatDescriptor);
	::SetPixelFormat(deviceContext, chosenFormat, &pixelFormatDescriptor);

	// Needed to make a modern context.
	HGLRC fakeContext = wglCreateContext(deviceContext);
	wglMakeCurrent(deviceContext, fakeContext);

	int32_t attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		NULL
	};

	PROC wglCreateContextAttribsProc = wglGetProcAddress("wglCreateContextAttribsARB");
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribs = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglCreateContextAttribsProc);

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

auto Renderer::InitRenderer(int32_t viewportWidth, int32_t viewportHeight, const RenderSettings& settings) -> bool
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
	for (uint32_t& bufferTexture : BufferTextures)
	{
		glGenTextures(1, &bufferTexture);

		glBindTexture(GL_TEXTURE_2D, bufferTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewportWidth, viewportHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	uint32_t vertexSize = 0;
	const char* vertexData = nullptr;

	// Load vertex shader text.
	RET_IF_COND_FALSE(LoadFileFromResource(IDR_VERTEXSHADER, vertexSize, vertexData));

	std::string vertexSource = GuaranteeNullTermination(vertexSize, vertexData);
	std::string fragmentSource;

	if (settings.MainPath.empty())
	{
		uint32_t fragmentSize = 0;
		const char* fragmentData = nullptr;

		// Load fragment shader text.
		RET_IF_COND_FALSE(LoadFileFromResource(IDR_FRAGMENTSHADER, fragmentSize, fragmentData));

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
	RET_IF_COND_FALSE(CreateShader(QuadShader, vertexSource, fragmentSource, commonSource));

	// Initialize main quad shader bindings.
	for (int32_t i = 0; i < CHANNEL_COUNT; i++)
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
			int32_t width, height;
			RET_IF_COND_FALSE(LoadTexture(mainChannel, QuadChannels[i], width, height));

			QuadChannelResolutions[i] = Vector3(width, height, 0);
		}

		if (Globals::ValidBindings.contains(mainChannel))
			QuadChannelResolutions[i] = Vector3(viewportWidth, viewportHeight, 0);
	}

	// Create buffers.
	uint32_t channels[CHANNEL_COUNT];
	Vector3 channelResolutions[CHANNEL_COUNT];

	if (!settings.BufferAPath.empty())
	{
		Buffers[0] = std::make_unique<Buffer>();

		std::string bufferSource = LoadFileFromDisk(settings.BufferAPath);
		std::unique_ptr<Shader> shader = std::make_unique<Shader>();

		RET_IF_COND_FALSE(CreateShader(shader, vertexSource, bufferSource, commonSource));
		RET_IF_COND_FALSE(Buffers[0]->SetupBuffer(&BufferTextures[0], ViewportWidth, ViewportHeight, BUFFERA_START, shader));

		// Let's initialize the channels.
		for (int32_t i = 0; i < CHANNEL_COUNT; i++)
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
				int32_t width, height;
				RET_IF_COND_FALSE(LoadTexture(bufferAChannel, channels[i], width, height));

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

		RET_IF_COND_FALSE(CreateShader(shader, vertexSource, bufferSource, commonSource));
		RET_IF_COND_FALSE(Buffers[1]->SetupBuffer(&BufferTextures[1], ViewportWidth, ViewportHeight, BUFFERB_START, shader));

		// Let's initialize the channels.
		for (int32_t i = 0; i < CHANNEL_COUNT; i++)
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
				int32_t width, height;
				RET_IF_COND_FALSE(LoadTexture(bufferBChannel, channels[i], width, height));

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

		RET_IF_COND_FALSE(CreateShader(shader, vertexSource, bufferSource, commonSource));
		RET_IF_COND_FALSE(Buffers[2]->SetupBuffer(&BufferTextures[2], ViewportWidth, ViewportHeight, BUFFERC_START, shader));

		// Let's initialize the channels.
		for (int32_t i = 0; i < CHANNEL_COUNT; i++)
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
				int32_t width, height;
				RET_IF_COND_FALSE(LoadTexture(bufferCChannel, channels[i], width, height));

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

		RET_IF_COND_FALSE(CreateShader(shader, vertexSource, bufferSource, commonSource));
		RET_IF_COND_FALSE(Buffers[3]->SetupBuffer(&BufferTextures[3], ViewportWidth, ViewportHeight, BUFFERD_START, shader));

		// Let's initialize the channels.
		for (int32_t i = 0; i < CHANNEL_COUNT; i++)
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
				int32_t width, height;
				RET_IF_COND_FALSE(LoadTexture(bufferDChannel, channels[i], width, height));

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

	// Initialize framerate limiter.
	frameLimiter = FrameLimiter(settings.FramerateCap);
	
	// Request the minimum possible resolution for periodic Win32 timers.
	frameLimiter.SetHighPrecisionTiming(true);

	// Disable V-sync to allow for arbitrary frametimes.
	wglSwapIntervalEXT(0);

	return true;
}

auto Renderer::DoRender(HDC deviceContext) -> void
{
	frameLimiter.StartTimer();

	FrameStartTime = GetUnixTimeInMs();

	std::time_t currentCTime = std::time(nullptr);
	std::tm* detailedTime = std::localtime(&currentCTime);

	ShaderUniforms.Time = (FrameStartTime - ProgramStart) / 1000.0f;
	ShaderUniforms.DeltaTime = ProgramDelta / 1000.0f;
	ShaderUniforms.FrameRate = 1000.0f / ProgramDelta;

	ShaderUniforms.Year = detailedTime->tm_year + 1900;
	ShaderUniforms.Month = detailedTime->tm_mon + 1;
	ShaderUniforms.Day = detailedTime->tm_mday;
	ShaderUniforms.Seconds = (detailedTime->tm_hour * 3600) + (detailedTime->tm_min * 60) + detailedTime->tm_sec;

	ShaderUniforms.FrameCount = FrameCount;

	glBindVertexArray(QuadVao);
	for (std::unique_ptr<Buffer>& buffer : Buffers)
	{
		if (buffer)
		{
			buffer->SetupRender(ShaderUniforms);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		}
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	for (int32_t i = 0; i < CHANNEL_COUNT; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, QuadChannels[i]);
	}

	QuadShader->SetFloatUniform("iTime", ShaderUniforms.Time);
	QuadShader->SetFloatUniform("iTimeDelta", ShaderUniforms.DeltaTime);
	QuadShader->SetFloatUniform("iFrameRate", ShaderUniforms.FrameRate);
	QuadShader->SetIntUniform("iFrame", FrameCount);

	QuadShader->SetVector4Uniform("iDate", ShaderUniforms.Year, ShaderUniforms.Month, ShaderUniforms.Day, ShaderUniforms.Seconds);
	QuadShader->UseShader();

	glBindVertexArray(QuadVao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	::SwapBuffers(deviceContext);

	frameLimiter.EndTimer();
	frameLimiter.WaitForNextFrame();

	uint32_t currentTime = GetUnixTimeInMs();
	ProgramDelta = currentTime - FrameStartTime;

	FrameCount++;
}

auto Renderer::UninitializeRenderer() -> void
{
	// Return to the default periodic timer resolution.
	frameLimiter.SetHighPrecisionTiming(false);

	glDeleteVertexArrays(1, &QuadVao);
	glDeleteBuffers(1, &QuadVbo);
	glDeleteBuffers(1, &QuadEbo);

	for (uint32_t& bufferTexture : BufferTextures)
	{
		glDeleteTextures(1, &bufferTexture);
	}

	for (uint32_t& quadChannel : QuadChannels)
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

auto LoadTexture(const std::string& filename, uint32_t& texture, int32_t& width, int32_t& height) -> bool
{
	stbi_set_flip_vertically_on_load(true);

	int32_t numberOfChannels;
	unsigned char* image = stbi_load(filename.c_str(), &width, &height, &numberOfChannels, STBI_rgb_alpha);

	RET_IF_COND_FALSE_MSG(image, "Could not load image file " + filename + ".");

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(image);

	return true;
}

auto LoadFileFromResource(int32_t resourceId, uint32_t& size, const char*& data) -> bool
{
	HMODULE moduleHandle = ::GetModuleHandle(nullptr);
	HRSRC resourceHandle = ::FindResource(moduleHandle, MAKEINTRESOURCE(resourceId), "TEXT");
	RET_IF_COND_FALSE(resourceHandle);

	HGLOBAL resourceData = ::LoadResource(moduleHandle, resourceHandle);
	RET_IF_COND_FALSE(resourceData);

	size = ::SizeofResource(moduleHandle, resourceHandle);
	data = static_cast<const char*>(::LockResource(resourceData));

	return true;
}

auto GuaranteeNullTermination(uint32_t size, const char* data) -> std::string
{
	std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size + 1);
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
	RET_IF_COND_FALSE(target->LoadShader(vertexSource));
	RET_IF_COND_FALSE(target->LoadShadertoyShader(fragmentSource, commonSource));
	RET_IF_COND_FALSE(target->CreateProgram());

	return true;
}

auto GetUnixTimeInMs() -> uint32_t
{
	SystemTimepoint currentPoint = std::chrono::system_clock::now();

	return std::chrono::duration_cast<std::chrono::milliseconds>(currentPoint.time_since_epoch()).count();
}