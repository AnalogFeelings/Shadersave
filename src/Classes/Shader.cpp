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

#include <Globals.h>
#include <Classes/Shader.h>

#include <sstream>

auto Shader::LoadShader(const std::string& vertexText) -> bool
{
	const char* constVertex = vertexText.c_str();
	unsigned int createdVertex = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(createdVertex, 1, &constVertex, nullptr);
	glCompileShader(createdVertex);

	bool vertexResult = this->CheckCompileErrors(createdVertex, "VERTEX");

	this->VertexShader = createdVertex;

	return vertexResult;
}

auto Shader::LoadShadertoyShader(std::string& fragmentText) -> bool
{
	std::stringstream stream;

	stream << "#version 430 core" << "\n";

	stream << "uniform vec3 iResolution;" << "\n";

	stream << "uniform float iTime;" << "\n";
	stream << "uniform float iTimeDelta;" << "\n";

	stream << "uniform float iFrameRate;" << "\n";
	stream << "uniform int iFrame;" << "\n";

	stream << "uniform sampler2D iChannel0;" << "\n";
	stream << "uniform sampler2D iChannel1;" << "\n";
	stream << "uniform sampler2D iChannel2;" << "\n";
	stream << "uniform sampler2D iChannel3;" << "\n";
	stream << "uniform float iChannelTime[4];" << "\n"; // This gets ignored but still needs a definition.
	stream << "uniform vec3 iChannelResolution[4];" << "\n";

	stream << "uniform vec4 iDate;" << "\n";
	stream << "uniform vec4 iMouse;" << "\n"; // This gets ignored but still needs a definition.
	
	stream << "out vec4 analogFeelings_Output;" << "\n";

	stream << fragmentText << "\n";

	stream << "void main() {" << "\n";
	stream << "    mainImage(analogFeelings_Output, gl_FragCoord.xy);" << "\n";
	stream << "}" << "\n";

	fragmentText = stream.str();

	const char* constFragment = fragmentText.c_str();
	unsigned int createdFragment = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(createdFragment, 1, &constFragment, nullptr);
	glCompileShader(createdFragment);

	bool fragmentResult = this->CheckCompileErrors(createdFragment, "FRAGMENT");

	this->FragmentShader = createdFragment;

	return fragmentResult;
}

auto Shader::CreateProgram() -> bool
{
	this->ProgramId = glCreateProgram();

	glAttachShader(this->ProgramId, this->VertexShader);
	glAttachShader(this->ProgramId, this->FragmentShader);
	glLinkProgram(this->ProgramId);

	bool programResult = this->CheckCompileErrors(this->ProgramId, "PROGRAM");
	if (!programResult)
		return false;

	glDeleteShader(this->VertexShader);
	glDeleteShader(this->FragmentShader);

	int uniformCount;
	glGetProgramiv(this->ProgramId, GL_ACTIVE_UNIFORMS, &uniformCount);

	for (int i = 0; i < uniformCount; i++)
	{
		int length, size;
		unsigned int type;
		char buffer[UNIFORM_BUFFER_SIZE] = {};

		glGetActiveUniform(this->ProgramId, i, UNIFORM_BUFFER_SIZE, &length, &size, &type, buffer);

		std::string bufferString(buffer);
		int location = glGetUniformLocation(this->ProgramId, buffer);

		if (bufferString.ends_with("[0]"))
			bufferString.resize(bufferString.size() - 3);

		this->UniformMap.insert(std::pair(bufferString, location));
	}

	return true;
}

auto Shader::UseShader() -> void
{
	glUseProgram(this->ProgramId);
}

Shader::~Shader()
{
	glDeleteProgram(this->ProgramId);
}

auto Shader::SetIntUniform(const std::string& name, int value) -> void
{
	if(!this->UniformMap.contains(name)) return;

	int uniformLocation = this->UniformMap.at(name);
	glProgramUniform1i(this->ProgramId, uniformLocation, value);
}

auto Shader::SetFloatUniform(const std::string& name, float value) -> void
{
	if(!this->UniformMap.contains(name)) return;

	int uniformLocation = this->UniformMap.at(name);
	glProgramUniform1f(this->ProgramId, uniformLocation, value);
}

auto Shader::SetVector2Uniform(const std::string& name, float x, float y) -> void
{
	if(!this->UniformMap.contains(name)) return;

	int uniformLocation = this->UniformMap.at(name);
	glProgramUniform2f(this->ProgramId, uniformLocation, x, y);
}

auto Shader::SetVector3Uniform(const std::string& name, float x, float y, float z) -> void
{
	if(!this->UniformMap.contains(name)) return;

	int uniformLocation = this->UniformMap.at(name);
	glProgramUniform3f(this->ProgramId, uniformLocation, x, y, z);
}

auto Shader::SetVector4Uniform(const std::string& name, float x, float y, float z, float w) -> void
{
	if(!this->UniformMap.contains(name)) return;

	int uniformLocation = this->UniformMap.at(name);
	glProgramUniform4f(this->ProgramId, uniformLocation, x, y, z, w);
}

auto Shader::SetVector3ArrayUniform(const std::string& name, Vector3* value, int size) -> void
{
	if (!this->UniformMap.contains(name)) return;

	int uniformLocation = this->UniformMap.at(name);
	glProgramUniform3fv(this->ProgramId, uniformLocation, size, reinterpret_cast<float*>(value));
}

auto Shader::CheckCompileErrors(unsigned int shaderId, const std::string& type) -> bool
{
	int isSuccess;

	if (type != "PROGRAM")
	{
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isSuccess);
		if (!isSuccess)
		{
			glGetShaderInfoLog(shaderId, SHADER_LOG_SIZE, nullptr, this->ShaderLog);
			Globals::LastError = this->ShaderLog;
		}
	}
	else
	{
		glGetProgramiv(shaderId, GL_LINK_STATUS, &isSuccess);
		if (!isSuccess)
		{
			glGetProgramInfoLog(shaderId, SHADER_LOG_SIZE, nullptr, this->ShaderLog);
			Globals::LastError = this->ShaderLog;
		}
	}

	return isSuccess;
}
