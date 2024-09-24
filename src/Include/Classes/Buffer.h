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

#include <memory>
#include <Classes/Shader.h>

class Buffer
{
public:
	/// <summary>
	/// Initializes the buffer. Sets LastError on failure.
	/// </summary>
	/// <param name="textureGlobal">A reference to the global texture variable.</param>
	/// <param name="viewportWidth">The viewport width.</param>
	/// <param name="viewportHeight">The viewport height.</param>
	/// <param name="channelStart">The starting number of the buffer channel texture unit.</param>
	/// <param name="shader">The shader to use.</param>
	/// <returns>TRUE if initialized successfully.</returns>
	auto SetupBuffer(PUINT textureGlobal, INT viewportWidth, INT viewportHeight, UINT channelStart, std::unique_ptr<Shader>& shader) -> BOOL;

	/// <summary>
	/// Sets up the state machine to render the buffer.
	/// </summary>
	/// <param name="uniforms">A reference to the uniforms to use.</param>
	auto SetupRender(PUNIFORMS uniforms) -> VOID;

	/// <summary>
	/// Destroys all resources.
	/// </summary>
	~Buffer();

private:
	UINT BufferFramebuffer = 0;
	UINT BufferTexture = 0;
	UINT BufferTextureCopy = 0;

	PUINT BufferTextureGlobal = nullptr;

	UINT ChannelStart = 0;

	std::unique_ptr<Shader> BufferShader;
};
