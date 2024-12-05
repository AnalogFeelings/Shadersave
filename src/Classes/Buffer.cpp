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

#include <Classes/Buffer.h>
#include <Globals.h>

#include <GL/glew.h>

auto Buffer::SetupBuffer(unsigned int* textureGlobal, int viewportWidth, int viewportHeight, int channelStart, std::unique_ptr<Shader>& shader) -> bool
{
	glGenFramebuffers(1, &this->BufferFramebuffer);
	glGenTextures(1, &this->BufferTexture);
	glGenTextures(1, &this->BufferTextureCopy);

	glBindTexture(GL_TEXTURE_2D, this->BufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewportWidth, viewportHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, this->BufferTextureCopy);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewportWidth, viewportHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, this->BufferFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->BufferTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Globals::LastError = "Framebuffer was not complete on initialization.";

		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->ViewportWidth = viewportWidth;
	this->ViewportHeight = viewportHeight;

	this->BufferShader = std::move(shader);
	this->BufferTextureGlobal = textureGlobal;

	this->ChannelStart = channelStart;

	// Set some uniforms ahead of time for performance.
	this->BufferShader->SetVector3Uniform("iResolution", ViewportWidth, ViewportHeight, 0);

	this->BufferShader->SetIntUniform("iChannel0", this->ChannelStart);
	this->BufferShader->SetIntUniform("iChannel1", this->ChannelStart + 1);
	this->BufferShader->SetIntUniform("iChannel2", this->ChannelStart + 2);
	this->BufferShader->SetIntUniform("iChannel3", this->ChannelStart + 3);

	return true;
}

auto Buffer::SetupChannels(const unsigned int(&channels)[CHANNEL_COUNT], const Vector3(&channelResolutions)[CHANNEL_COUNT]) -> void
{
	std::memcpy(this->Channels, channels, sizeof(unsigned int) * CHANNEL_COUNT);
	std::memcpy(this->ChannelResolutions, channelResolutions, sizeof(Vector3) * CHANNEL_COUNT);

	this->BufferShader->SetVector3ArrayUniform("iChannelResolution", this->ChannelResolutions, CHANNEL_COUNT);
}

auto Buffer::SetupRender(const Uniforms& uniforms) -> void
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->BufferFramebuffer);

	if (this->BufferTextureLast == this->BufferTexture)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->BufferTextureCopy, 0);

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		this->BufferTextureLast = this->BufferTextureCopy;

		glCopyImageSubData(this->BufferTexture, GL_TEXTURE_2D, 0, 0, 0, 0,
			*this->BufferTextureGlobal, GL_TEXTURE_2D, 0, 0, 0, 0,
			this->ViewportWidth, this->ViewportHeight, 1);
	}
	else
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->BufferTexture, 0);

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		this->BufferTextureLast = this->BufferTexture;

		glCopyImageSubData(this->BufferTextureCopy, GL_TEXTURE_2D, 0, 0, 0, 0,
			*this->BufferTextureGlobal, GL_TEXTURE_2D, 0, 0, 0, 0,
			this->ViewportWidth, this->ViewportHeight, 1);
	}

	this->BufferShader->SetFloatUniform("iTime", uniforms.Time);
	this->BufferShader->SetFloatUniform("iTimeDelta", uniforms.DeltaTime);
	this->BufferShader->SetFloatUniform("iFrameRate", uniforms.FrameRate);
	this->BufferShader->SetIntUniform("iFrame", uniforms.FrameCount);
	this->BufferShader->SetVector4Uniform("iDate", uniforms.Year, uniforms.Month, uniforms.Day, uniforms.Seconds);

	this->BufferShader->UseShader();

	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		glActiveTexture(GL_TEXTURE0 + this->ChannelStart + i);
		glBindTexture(GL_TEXTURE_2D, this->Channels[i]);
	}
}

Buffer::~Buffer()
{
	glDeleteFramebuffers(1, &this->BufferFramebuffer);

	glDeleteTextures(1, &this->BufferTexture);
	glDeleteTextures(1, &this->BufferTextureCopy);
}