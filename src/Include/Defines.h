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

#include <chrono>

#define SHADERSAVE_VERSION "0.6"

#define GLEW_ERROR_SIZE 128
#define OPENGL_ERROR_SIZE 128
#define SHADER_LOG_SIZE 1024
#define UNIFORM_BUFFER_SIZE 16

#define BUFFERA_START 4
#define BUFFERB_START 8
#define BUFFERC_START 12
#define BUFFERD_START 16

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
	int ViewportWidth;
	int ViewportHeight;

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
	std::string MainPath;
	std::string BufferAPath;
	std::string BufferBPath;
	std::string BufferCPath;
	std::string BufferDPath;
	std::string CommonPath;

	std::string MainChannel0;
	std::string MainChannel1;
	std::string MainChannel2;
	std::string MainChannel3;

	std::string BufferAChannel0;
	std::string BufferAChannel1;
	std::string BufferAChannel2;
	std::string BufferAChannel3;

	std::string BufferBChannel0;
	std::string BufferBChannel1;
	std::string BufferBChannel2;
	std::string BufferBChannel3;

	std::string BufferCChannel0;
	std::string BufferCChannel1;
	std::string BufferCChannel2;
	std::string BufferCChannel3;

	std::string BufferDChannel0;
	std::string BufferDChannel1;
	std::string BufferDChannel2;
	std::string BufferDChannel3;
};
