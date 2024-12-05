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

#include <Defines.h>

#include <string>
#include <unordered_map>

class Shader
{
public:
	char ShaderLog[SHADER_LOG_SIZE] = {};

	auto LoadShader(const std::string& vertexText) -> bool;
	auto LoadShadertoyShader(std::string& fragmentText, const std::string& commonText) -> bool;
	auto CreateProgram() -> bool;
	auto UseShader() -> void;
	~Shader();

	auto SetIntUniform(const std::string& name, int value) -> void;
	auto SetFloatUniform(const std::string& name, float value) -> void;
	auto SetVector2Uniform(const std::string& name, float x, float y) -> void;
	auto SetVector3Uniform(const std::string& name, float x, float y, float z) -> void;
	auto SetVector4Uniform(const std::string& name, float x, float y, float z, float w) -> void;
	auto SetVector3ArrayUniform(const std::string& name, Vector3* value, int size) -> void;

private:
	unsigned int ProgramId = 0;
	unsigned int VertexShader = 0;
	unsigned int FragmentShader = 0;

	std::unordered_map<std::string, int> UniformMap = {};

	/// <summary>
	/// Checks if a shader has compile errors. Sets LastError on failure.
	/// </summary>
	/// <param name="shaderId">The ID of the shader object.</param>
	/// <param name="type">The type of the shader object.</param>
	/// <returns>TRUE if there are no errors.</returns>
	auto CheckCompileErrors(unsigned int shaderId, const std::string& type) -> bool;
};
