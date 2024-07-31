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

#define SHADERSAVE_VERSION "0.3"
#define GLEW_ERROR_SIZE 128
#define OPENGL_ERROR_SIZE 128

#define REGISTRY_SUBKEY "SOFTWARE\\AnalogFeelings\\Shadersave"

#define FRAMERATE_CAP "FramerateCap"
#define SHADER_PATH "ShaderPath"
#define BUFFERA_PATH "BufferAPath"
#define BUFFERB_PATH "BufferBPath"
#define BUFFERC_PATH "BufferCPath"
#define BUFFERD_PATH "BufferDPath"

#define CHANNEL0_BINDING "Channel0Binding"
#define CHANNEL1_BINDING "Channel1Binding"
#define CHANNEL2_BINDING "Channel2Binding"
#define CHANNEL3_BINDING "Channel3Binding"

#define BUFFER_A "BUFFER_A"
#define BUFFER_B "BUFFER_B"
#define BUFFER_C "BUFFER_C"
#define BUFFER_D "BUFFER_D"

typedef std::chrono::time_point<std::chrono::system_clock> TIMEPOINT;
