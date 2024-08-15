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
#include <core.h>

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

	// Needed to make a modern context.
	HGLRC fakeContext = wglCreateContext(deviceContext);
	wglMakeCurrent(deviceContext, fakeContext);

	INT attribs[] =
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
		return FALSE;
	}

	return TRUE;
}

auto GLRenderer::InitRenderer(INT viewportWidth, INT viewportHeight, CONST SETTINGS& settings) -> BOOL
{
	this->ViewportWidth = viewportWidth;
	this->ViewportHeight = viewportHeight;

	glViewport(0, 0, this->ViewportWidth, this->ViewportHeight);

	glGenVertexArrays(1, &this->QuadVao);
	this->LabelObject(GL_VERTEX_ARRAY, this->QuadVao, "Screen VAO");

	glGenBuffers(1, &this->QuadVbo);
	this->LabelObject(GL_BUFFER, this->QuadVbo, "Screen VBO");

	glGenBuffers(1, &this->QuadEbo);
	this->LabelObject(GL_BUFFER, this->QuadEbo, "Screen EBO");

	glBindVertexArray(this->QuadVao);

	glBindBuffer(GL_ARRAY_BUFFER, this->QuadVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTICES), QUAD_VERTICES, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->QuadEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_INDICES), QUAD_INDICES, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(FLOAT), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Generate framebuffers.
	if (!settings.BufferAPath.empty())
	{
		BOOL result = this->GenerateFramebuffer(&this->BufferAFramebuffer, &this->BufferATexture);
		if (!result)
			return FALSE;

		this->LabelObject(GL_FRAMEBUFFER, this->BufferAFramebuffer, "BufferA Framebuffer");
		this->LabelObject(GL_TEXTURE, this->BufferATexture, "BufferA Texture");
	}
	if (!settings.BufferBPath.empty())
	{
		BOOL result = this->GenerateFramebuffer(&this->BufferBFramebuffer, &this->BufferBTexture);
		if (!result)
			return FALSE;

		this->LabelObject(GL_FRAMEBUFFER, this->BufferBFramebuffer, "BufferB Framebuffer");
		this->LabelObject(GL_TEXTURE, this->BufferBTexture, "BufferB Texture");
	}
	if (!settings.BufferCPath.empty())
	{
		BOOL result = this->GenerateFramebuffer(&this->BufferCFramebuffer, &this->BufferCTexture);
		if (!result)
			return FALSE;

		this->LabelObject(GL_FRAMEBUFFER, this->BufferCFramebuffer, "BufferC Framebuffer");
		this->LabelObject(GL_TEXTURE, this->BufferCTexture, "BufferC Texture");
	}
	if (!settings.BufferDPath.empty())
	{
		BOOL result = this->GenerateFramebuffer(&this->BufferDFramebuffer, &this->BufferDTexture);
		if (!result)
			return FALSE;

		this->LabelObject(GL_FRAMEBUFFER, this->BufferDFramebuffer, "BufferD Framebuffer");
		this->LabelObject(GL_TEXTURE, this->BufferDTexture, "BufferD Texture");
	}

	UINT vertexSize;
	PCSTR vertexData;

	// Load vertex shader text.
	BOOL vertexResult = this->LoadFileFromResource(IDR_VERTEXSHADER, vertexSize, vertexData);
	if (!vertexResult)
		return FALSE;

	std::string vertexSource = this->GuaranteeNullTermination(vertexSize, vertexData);
	std::string fragmentSource;

	if (settings.MainPath.empty())
	{
		UINT fragmentSize;
		PCSTR fragmentData;

		// Load fragment shader text.
		BOOL fragmentResult = this->LoadFileFromResource(IDR_FRAGMENTSHADER, fragmentSize, fragmentData);
		if (!fragmentResult)
			return FALSE;

		fragmentSource = this->GuaranteeNullTermination(fragmentSize, fragmentData);
	}
	else
	{
		fragmentSource = this->LoadFileFromDisk(settings.MainPath);
	}

	// Actually load the shaders and compile them.
	BOOL quadResult = this->CreateShader(this->QuadShader, vertexSource, fragmentSource);
	if (!quadResult)
		return FALSE;

	// Create buffer shaders.
	if (!settings.BufferAPath.empty())
	{
		std::string bufferASource = this->LoadFileFromDisk(settings.BufferAPath);

		BOOL bufferAResult = this->CreateShader(this->BufferAShader, vertexSource, bufferASource);
		if (!bufferAResult)
			return FALSE;

		if (settings.Channel0 == BUFFER_A)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->BufferATexture);
			this->Channel0Texture = this->BufferATexture;

			glGenTextures(1, &this->Channel0TextureCopy);
			glBindTexture(GL_TEXTURE_2D, this->Channel0TextureCopy);

			this->LabelObject(GL_TEXTURE, this->Channel0TextureCopy, "Channel 0 Copy");

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->ViewportWidth, this->ViewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		if (settings.Channel1 == BUFFER_A)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, this->BufferATexture);
			this->Channel1Texture = this->BufferATexture;
		}
		if (settings.Channel2 == BUFFER_A)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, this->BufferATexture);
			this->Channel1Texture = this->BufferATexture;
		}
		if (settings.Channel3 == BUFFER_A)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, this->BufferATexture);
			this->Channel1Texture = this->BufferATexture;
		}
	}
	if (!settings.BufferBPath.empty())
	{
		std::string bufferBSource = this->LoadFileFromDisk(settings.BufferBPath);

		BOOL bufferBResult = this->CreateShader(this->BufferBShader, vertexSource, bufferBSource);
		if (!bufferBResult)
			return FALSE;

		if (settings.Channel0 == BUFFER_B)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->BufferBTexture);
			this->Channel0Texture = this->BufferBTexture;
		}
		if (settings.Channel1 == BUFFER_B)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, this->BufferBTexture);
			this->Channel1Texture = this->BufferBTexture;
		}
		if (settings.Channel2 == BUFFER_B)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, this->BufferBTexture);
			this->Channel2Texture = this->BufferBTexture;
		}
		if (settings.Channel3 == BUFFER_B)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, this->BufferBTexture);
			this->Channel3Texture = this->BufferBTexture;
		}
	}
	if (!settings.BufferCPath.empty())
	{
		std::string bufferCSource = this->LoadFileFromDisk(settings.BufferCPath);

		BOOL bufferCResult = this->CreateShader(this->BufferCShader, vertexSource, bufferCSource);
		if (!bufferCResult)
			return FALSE;

		if (settings.Channel0 == BUFFER_C)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->BufferCTexture);
			this->Channel0Texture = this->BufferCTexture;
		}
		if (settings.Channel1 == BUFFER_C)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, this->BufferCTexture);
			this->Channel1Texture = this->BufferCTexture;
		}
		if (settings.Channel2 == BUFFER_C)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, this->BufferCTexture);
			this->Channel2Texture = this->BufferCTexture;
		}
		if (settings.Channel3 == BUFFER_C)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, this->BufferCTexture);
			this->Channel3Texture = this->BufferCTexture;
		}
	}
	if (!settings.BufferDPath.empty())
	{
		std::string bufferDSource = this->LoadFileFromDisk(settings.BufferDPath);

		BOOL bufferDResult = this->CreateShader(this->BufferDShader, vertexSource, bufferDSource);
		if (!bufferDResult)
			return FALSE;

		if (settings.Channel0 == BUFFER_D)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->BufferDTexture);
			this->Channel0Texture = this->BufferDTexture;
		}
		if (settings.Channel1 == BUFFER_D)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, this->BufferDTexture);
			this->Channel1Texture = this->BufferDTexture;
		}
		if (settings.Channel2 == BUFFER_D)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, this->BufferDTexture);
			this->Channel2Texture = this->BufferDTexture;
		}
		if (settings.Channel3 == BUFFER_D)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, this->BufferDTexture);
			this->Channel3Texture = this->BufferDTexture;
		}
	}

	// Set up startup time.
	this->ProgramStart = this->GetUnixTimeInMs();

	return TRUE;
}

auto GLRenderer::DoRender(HDC deviceContext) -> VOID
{
	this->ProgramNow = this->GetUnixTimeInMs();

	std::time_t currentCTime = std::time(nullptr);
	std::tm* detailedTime = std::localtime(&currentCTime);

	UNIFORMS uniforms =
	{
		.Time = (this->ProgramNow - this->ProgramStart) / 1000.0f,
		.DeltaTime = this->ProgramDelta / 1000.0f,
		.FrameRate = 1000.0f / this->ProgramDelta,

		.Year = detailedTime->tm_year + 1900,
		.Month = detailedTime->tm_mon + 1,
		.Day = detailedTime->tm_mday,
		.Seconds = (detailedTime->tm_hour * 3600) + (detailedTime->tm_min * 60) + detailedTime->tm_sec
	};

	if (this->BufferAFramebuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, this->BufferAFramebuffer);

		if(this->Channel0LastFrame == this->Channel0Texture)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Channel0TextureCopy, 0);

			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->Channel0Texture);

			this->Channel0LastFrame = this->Channel0TextureCopy;
		}
		else
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Channel0Texture, 0);

			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->Channel0TextureCopy);

			this->Channel0LastFrame = this->Channel0Texture;
		}
		
		this->BufferAShader->UseShader();
		this->SetUniformValues(this->BufferAShader, &uniforms);

		glBindVertexArray(this->QuadVao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->Channel0Texture);

	this->QuadShader->UseShader();
	this->SetUniformValues(this->QuadShader, &uniforms);

	glBindVertexArray(this->QuadVao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	::SwapBuffers(deviceContext);

	ULONG64 currentTime = this->GetUnixTimeInMs();
	this->ProgramDelta = currentTime - this->ProgramNow;

	this->FrameCount++;
}

GLRenderer::~GLRenderer()
{
	glDeleteVertexArrays(1, &this->QuadVao);
	glDeleteBuffers(1, &this->QuadVbo);
	glDeleteBuffers(1, &this->QuadEbo);

	glDeleteFramebuffers(1, &this->BufferAFramebuffer);
	glDeleteFramebuffers(1, &this->BufferBFramebuffer);
	glDeleteFramebuffers(1, &this->BufferCFramebuffer);
	glDeleteFramebuffers(1, &this->BufferDFramebuffer);

	glDeleteTextures(1, &this->BufferATexture);
	glDeleteTextures(1, &this->BufferBTexture);
	glDeleteTextures(1, &this->BufferCTexture);
	glDeleteTextures(1, &this->BufferDTexture);

	glDeleteTextures(1, &this->Channel0Texture);
	glDeleteTextures(1, &this->Channel1Texture);
	glDeleteTextures(1, &this->Channel2Texture);
	glDeleteTextures(1, &this->Channel3Texture);

	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(GlRenderContext);
	::ReleaseDC(MainWindow, DeviceContext);
}

auto GLRenderer::SetUniformValues(std::shared_ptr<Shader> target, CONST PUNIFORMS uniforms) -> VOID
{
	target->SetVector3Uniform("iResolution", this->ViewportWidth, this->ViewportHeight, 0);
	target->SetFloatUniform("iTime", uniforms->Time);
	target->SetFloatUniform("iTimeDelta", uniforms->DeltaTime);
	target->SetFloatUniform("iFrameRate", uniforms->FrameRate);
	target->SetIntUniform("iFrame", this->FrameCount);

	// Set texture units.
	target->SetIntUniform("iChannel0", 0);
	target->SetIntUniform("iChannel1", 1);
	target->SetIntUniform("iChannel2", 2);
	target->SetIntUniform("iChannel3", 3);

	target->SetVector4Uniform("iDate", uniforms->Year, uniforms->Month, uniforms->Day, uniforms->Seconds);
}

auto GLRenderer::GenerateFramebuffer(PUINT targetFramebuffer, PUINT targetTexture) -> BOOL
{
	glGenFramebuffers(1, targetFramebuffer);
	glGenTextures(1, targetTexture);

	glBindFramebuffer(GL_FRAMEBUFFER, *targetFramebuffer);
	glBindTexture(GL_TEXTURE_2D, *targetTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->ViewportWidth, this->ViewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *targetTexture, 0);

	return TRUE;
}

auto GLRenderer::LabelObject(UINT type, UINT object, PCSTR label) -> VOID
{
	UINT stringLength = std::strlen(label);

	glObjectLabel(type, object, stringLength, label);
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

auto GLRenderer::GuaranteeNullTermination(UINT size, CONST PCSTR& data) -> std::string
{
	std::unique_ptr<CHAR[]> buffer = std::make_unique<CHAR[]>(size + 1);
	std::memcpy(buffer.get(), data, size);

	buffer.get()[size] = NULL;

	return std::string(buffer.get());
}

auto GLRenderer::LoadFileFromDisk(CONST std::string& filename) -> std::string
{
	std::ifstream fileStream(filename);
	std::stringstream stringStream;
	stringStream << fileStream.rdbuf();

	std::string text = stringStream.str();

	fileStream.close();

	return text;
}

auto GLRenderer::CreateShader(std::shared_ptr<Shader>& target, CONST std::string& vertexSource, std::string& fragmentSource) -> BOOL
{
	target = std::make_shared<Shader>();
	BOOL shaderResult = target->LoadShader(vertexSource);
	if (!shaderResult)
		return FALSE;

	BOOL shadertoyResult = target->LoadShadertoyShader(fragmentSource);
	if (!shadertoyResult)
		return FALSE;

	BOOL compileResult = target->CreateProgram();
	if (!compileResult)
		return FALSE;

	return TRUE;
}

auto GLRenderer::GetUnixTimeInMs() -> ULONG64
{
	TIMEPOINT currentPoint = std::chrono::system_clock::now();

	return std::chrono::duration_cast<std::chrono::milliseconds>(currentPoint.time_since_epoch()).count();
}