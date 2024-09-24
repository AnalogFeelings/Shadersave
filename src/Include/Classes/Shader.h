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

#include <windows.h>
#include <GL/glew.h>
#include <string>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <Defines.h>

class Shader
{
public:
	CHAR ShaderLog[SHADER_LOG_SIZE] = {};

	auto LoadShader(CONST std::string& vertexText) -> BOOL;
	auto LoadShadertoyShader(std::string& fragmentText) -> BOOL;
	auto CreateProgram() -> BOOL;
	auto UseShader() -> VOID;
	~Shader();

	auto SetIntUniform(CONST std::string& name, INT value) -> VOID;
	auto SetFloatUniform(CONST std::string& name, FLOAT value) -> VOID;
	auto SetVector2Uniform(CONST std::string& name, FLOAT x, FLOAT y) -> VOID;
	auto SetVector3Uniform(CONST std::string& name, FLOAT x, FLOAT y, FLOAT z) -> VOID;
	auto SetVector4Uniform(CONST std::string& name, FLOAT x, FLOAT y, FLOAT z, FLOAT w) -> VOID;

private:
	UINT ProgramId = 0;
	UINT VertexShader = 0;
	UINT FragmentShader = 0;

	std::unordered_map<std::string, INT> UniformMap = {};

	auto CheckCompileErrors(GLuint shaderId, CONST std::string& type) -> BOOL;
};
