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

#include <chrono>

#define SHADERSAVE_VERSION "1.2"

#define GLEW_ERROR_SIZE 128
#define OPENGL_ERROR_SIZE 128
#define SHADER_LOG_SIZE 1024
#define UNIFORM_BUFFER_SIZE 24

#define BUFFERA_START 4
#define BUFFERB_START 8
#define BUFFERC_START 12
#define BUFFERD_START 16

#define BUFFER_COUNT 4
#define CHANNEL_COUNT 4

#define REGISTRY_SUBKEY "SOFTWARE\\AnalogFeelings\\Shadersave"

#define FRAMERATE_CAP "FramerateCap"
#define SHADER_PATH "ShaderPath"
#define BUFFERA_PATH "BufferAPath"
#define BUFFERB_PATH "BufferBPath"
#define BUFFERC_PATH "BufferCPath"
#define BUFFERD_PATH "BufferDPath"
#define COMMON_PATH "CommonPath"

#define SHADER_CHANNEL0_BINDING "ShaderChannel0"
#define SHADER_CHANNEL1_BINDING "ShaderChannel1"
#define SHADER_CHANNEL2_BINDING "ShaderChannel2"
#define SHADER_CHANNEL3_BINDING "ShaderChannel3"

#define BUFFERA_CHANNEL0_BINDING "BufferAChannel0"
#define BUFFERA_CHANNEL1_BINDING "BufferAChannel1"
#define BUFFERA_CHANNEL2_BINDING "BufferAChannel2"
#define BUFFERA_CHANNEL3_BINDING "BufferAChannel3"

#define BUFFERB_CHANNEL0_BINDING "BufferBChannel0"
#define BUFFERB_CHANNEL1_BINDING "BufferBChannel1"
#define BUFFERB_CHANNEL2_BINDING "BufferBChannel2"
#define BUFFERB_CHANNEL3_BINDING "BufferBChannel3"

#define BUFFERC_CHANNEL0_BINDING "BufferCChannel0"
#define BUFFERC_CHANNEL1_BINDING "BufferCChannel1"
#define BUFFERC_CHANNEL2_BINDING "BufferCChannel2"
#define BUFFERC_CHANNEL3_BINDING "BufferCChannel3"

#define BUFFERD_CHANNEL0_BINDING "BufferDChannel0"
#define BUFFERD_CHANNEL1_BINDING "BufferDChannel1"
#define BUFFERD_CHANNEL2_BINDING "BufferDChannel2"
#define BUFFERD_CHANNEL3_BINDING "BufferDChannel3"

#define BUFFER_A "BUFFER_A"
#define BUFFER_B "BUFFER_B"
#define BUFFER_C "BUFFER_C"
#define BUFFER_D "BUFFER_D"

typedef std::chrono::time_point<std::chrono::system_clock> Timepoint;

struct Uniforms
{
	float Time;
	float DeltaTime;
	float FrameRate;

	int Year;
	int Month;
	int Day;
	int Seconds;

	int FrameCount;
};

struct RenderSettings
{
	std::string CommonPath;

	std::string MainPath;
	std::string MainChannels[CHANNEL_COUNT];

	std::string BufferAPath;
	std::string BufferAChannels[CHANNEL_COUNT];

	std::string BufferBPath;
	std::string BufferBChannels[CHANNEL_COUNT];

	std::string BufferCPath;
	std::string BufferCChannels[CHANNEL_COUNT];

	std::string BufferDPath;
	std::string BufferDChannels[CHANNEL_COUNT];
};

struct Vector3
{
	float X = 0;
	float Y = 0;
	float Z = 0;

	Vector3() = default;

	Vector3(float x, float y, float z) : X(x), Y(y), Z(z)
	{

	}
};
