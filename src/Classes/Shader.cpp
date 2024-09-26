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

#include <Classes/Shader.h>

#include <sstream>

auto Shader::LoadShader(CONST std::string& vertexText) -> BOOL
{
	PCSTR constVertex = vertexText.c_str();
	UINT createdVertex = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(createdVertex, 1, &constVertex, nullptr);
	glCompileShader(createdVertex);

	BOOL vertexResult = this->CheckCompileErrors(createdVertex, "VERTEX");

	this->VertexShader = createdVertex;

	return vertexResult;
}

auto Shader::LoadShadertoyShader(std::string& fragmentText) -> BOOL
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

	PCSTR constFragment = fragmentText.c_str();
	UINT createdFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(createdFragment, 1, &constFragment, nullptr);
	glCompileShader(createdFragment);
	BOOL fragmentResult = this->CheckCompileErrors(createdFragment, "FRAGMENT");

	this->FragmentShader = createdFragment;

	return fragmentResult;
}

auto Shader::CreateProgram() -> BOOL
{
	this->ProgramId = glCreateProgram();

	glAttachShader(this->ProgramId, this->VertexShader);
	glAttachShader(this->ProgramId, this->FragmentShader);
	glLinkProgram(this->ProgramId);

	BOOL programResult = this->CheckCompileErrors(this->ProgramId, "PROGRAM");

	glDeleteShader(this->VertexShader);
	glDeleteShader(this->FragmentShader);

	INT uniformCount;
	glGetProgramiv(this->ProgramId, GL_ACTIVE_UNIFORMS, &uniformCount);

	for (INT i = 0; i < uniformCount; i++)
	{
		INT length, size;
		UINT type;
		CHAR buffer[UNIFORM_BUFFER_SIZE] = {};

		glGetActiveUniform(this->ProgramId, i, UNIFORM_BUFFER_SIZE, &length, &size, &type, buffer);

		std::string bufferString(buffer);
		INT location = glGetUniformLocation(this->ProgramId, buffer);

		this->UniformMap.insert(std::pair(bufferString, location));
	}

	return programResult;
}

auto Shader::UseShader() -> VOID
{
	glUseProgram(this->ProgramId);
}

Shader::~Shader()
{
	glDeleteProgram(this->ProgramId);
}

auto Shader::SetIntUniform(CONST std::string& name, INT value) -> VOID
{
	if(!this->UniformMap.contains(name)) return;

	INT uniformLocation = this->UniformMap.at(name);
	glProgramUniform1i(this->ProgramId, uniformLocation, value);
}

auto Shader::SetFloatUniform(CONST std::string& name, FLOAT value) -> VOID
{
	if(!this->UniformMap.contains(name)) return;

	INT uniformLocation = this->UniformMap.at(name);
	glProgramUniform1f(this->ProgramId, uniformLocation, value);
}

auto Shader::SetVector2Uniform(CONST std::string& name, FLOAT x, FLOAT y) -> VOID
{
	if(!this->UniformMap.contains(name)) return;

	INT uniformLocation = this->UniformMap.at(name);
	glProgramUniform2f(this->ProgramId, uniformLocation, x, y);
}

auto Shader::SetVector3Uniform(CONST std::string& name, FLOAT x, FLOAT y, FLOAT z) -> VOID
{
	if(!this->UniformMap.contains(name)) return;

	INT uniformLocation = this->UniformMap.at(name);
	glProgramUniform3f(this->ProgramId, uniformLocation, x, y, z);
}

auto Shader::SetVector4Uniform(CONST std::string& name, FLOAT x, FLOAT y, FLOAT z, FLOAT w) -> VOID
{
	if(!this->UniformMap.contains(name)) return;

	INT uniformLocation = this->UniformMap.at(name);
	glProgramUniform4f(this->ProgramId, uniformLocation, x, y, z, w);
}

auto Shader::CheckCompileErrors(GLuint shaderId, CONST std::string& type) -> BOOL
{
	GLint isSuccess;

	if (type != "PROGRAM")
	{
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isSuccess);
		if (!isSuccess)
		{
			glGetShaderInfoLog(shaderId, SHADER_LOG_SIZE, nullptr, this->ShaderLog);
		}
	}
	else
	{
		glGetProgramiv(shaderId, GL_LINK_STATUS, &isSuccess);
		if (!isSuccess)
		{
			glGetProgramInfoLog(shaderId, SHADER_LOG_SIZE, nullptr, this->ShaderLog);
		}
	}

	return isSuccess;
}
