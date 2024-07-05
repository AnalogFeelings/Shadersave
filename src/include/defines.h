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

#define SHADERSAVE_VERSION "0.1"
#define GLEW_ERROR_SIZE 128
#define OPENGL_ERROR_SIZE 2048

#define REGISTRY_SUBKEY "SOFTWARE\\AnalogFeelings\\Shadersave"
#define FRAMERATE_CAP "FramerateCap"
#define SHADER_PATH "ShaderPath"

typedef std::chrono::time_point<std::chrono::system_clock> TIMEPOINT;
