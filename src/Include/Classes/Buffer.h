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

#pragma once

#include <Classes/Shader.h>
#include <Defines.h>

#include <memory>
#include <stdint.h>

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
	/// <returns>true if initialized successfully.</returns>
	auto SetupBuffer(uint32_t* textureGlobal, int32_t viewportWidth, int32_t viewportHeight, int32_t channelStart, std::unique_ptr<Shader>& shader) -> bool;

	/// <summary>
	/// Initializes the channel bindings.
	/// </summary>
	/// <param name="channels">The texture ID array for the channels.</param>
	/// <param name="channelResolutions">The channel resolutions array.</param>
	/// <returns></returns>
	auto SetupChannels(const uint32_t(&channels)[CHANNEL_COUNT], const Vector3 (&channelResolutions)[CHANNEL_COUNT]) -> void;

	/// <summary>
	/// Sets up the state machine to render the buffer.
	/// </summary>
	/// <param name="uniforms">A reference to the uniforms to use.</param>
	auto SetupRender(const Uniforms& uniforms) -> void;

	/// <summary>
	/// Destroys all resources.
	/// </summary>
	~Buffer();

private:
	int32_t ViewportWidth = 0;
	int32_t ViewportHeight = 0;
	int32_t ChannelStart = 0;

	uint32_t BufferFramebuffer = 0;
	uint32_t BufferTexture = 0;
	uint32_t BufferTextureCopy = 0;
	uint32_t BufferTextureLast = 0;

	uint32_t* BufferTextureGlobal = nullptr;

	uint32_t Channels[CHANNEL_COUNT] = {};
	Vector3 ChannelResolutions[CHANNEL_COUNT] = {};

	std::unique_ptr<Shader> BufferShader;
};
