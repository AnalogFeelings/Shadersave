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

#include <Classes/Buffer.h>
#include <Globals.h>

auto Buffer::SetupBuffer(PUINT textureGlobal, INT viewportWidth, INT viewportHeight, UINT channelStart, std::unique_ptr<Shader>& shader) -> BOOL
{
	glGenFramebuffers(1, &this->BufferFramebuffer);
	glGenTextures(1, &this->BufferTexture);
	glGenTextures(1, &this->BufferTextureCopy);

	glBindTexture(GL_TEXTURE_2D, this->BufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportWidth, viewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, this->BufferTextureCopy);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportWidth, viewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, this->BufferFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->BufferTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Globals::LastError = "Framebuffer was not complete on initialization.";
		return FALSE;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->BufferShader = std::move(shader);
	this->BufferTextureGlobal = textureGlobal;
	this->ChannelStart = channelStart;

	return TRUE;
}

auto Buffer::SetupRender(PUNIFORMS uniforms) -> VOID
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->BufferFramebuffer);

	if (*this->BufferTextureGlobal == this->BufferTexture)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->BufferTextureCopy, 0);

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, this->BufferTexture);

		*this->BufferTextureGlobal = this->BufferTextureCopy;
	}
	else
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->BufferTexture, 0);

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, this->BufferTextureCopy);

		*this->BufferTextureGlobal = this->BufferTexture;
	}

	this->BufferShader->SetVector3Uniform("iResolution", uniforms->ViewportWidth, uniforms->ViewportHeight, 0);
	this->BufferShader->SetFloatUniform("iTime", uniforms->Time);
	this->BufferShader->SetFloatUniform("iTimeDelta", uniforms->DeltaTime);
	this->BufferShader->SetFloatUniform("iFrameRate", uniforms->FrameRate);
	this->BufferShader->SetIntUniform("iFrame", uniforms->FrameCount);
	this->BufferShader->SetVector4Uniform("iDate", uniforms->Year, uniforms->Month, uniforms->Day, uniforms->Seconds);

	// Set texture units.
	this->BufferShader->SetIntUniform("iChannel0", this->ChannelStart);
	this->BufferShader->SetIntUniform("iChannel1", this->ChannelStart + 1);
	this->BufferShader->SetIntUniform("iChannel2", this->ChannelStart + 2);
	this->BufferShader->SetIntUniform("iChannel3", this->ChannelStart + 3);

	this->BufferShader->UseShader();
}

Buffer::~Buffer()
{
	glDeleteFramebuffers(1, &this->BufferFramebuffer);

	glDeleteTextures(1, &this->BufferTexture);
	glDeleteTextures(1, &this->BufferTextureCopy);
}