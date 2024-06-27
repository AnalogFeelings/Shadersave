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

#include "include/shader.h"

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
    std::regex entryRegex(R"(void\s+mainImage\s*\(\s*(?:out)?\s+vec4\s+([^,]+),\s*(?:in)?\s+vec2\s+([^)]+)\))");
    std::smatch matches;

    if(!std::regex_search(fragmentText, matches, entryRegex))
        return FALSE;

    std::string fragColorName = matches[1];
    std::string fragCoordName = matches[2];

    stream << "#version 430 core" << "\n";
    stream << "#define " << fragCoordName << " gl_FragCoord.xy" << "\n";
    stream << "uniform vec2 iResolution;" << "\n";
    stream << "uniform float iTime;" << "\n";
    stream << "uniform vec4 iMouse;" << "\n"; // This gets ignored but still needs a definition.
    stream << "out vec4 " << fragColorName << ";" << "\n";

    fragmentText = std::regex_replace(fragmentText, entryRegex, "void main()");

    std::string stringifiedStream = stream.str();
    fragmentText.insert(0, stringifiedStream);

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

    return programResult;
}

auto Shader::UseShader() -> VOID
{
    glUseProgram(this->ProgramId);
}

auto Shader::SetBoolUniform(CONST std::string& name, BOOL value) -> VOID
{
    GLint uniformLocation = glGetUniformLocation(this->ProgramId, name.c_str());
    glUniform1i(uniformLocation, (INT)value);
}

auto Shader::SetIntUniform(CONST std::string& name, INT value) -> VOID
{
    GLint uniformLocation = glGetUniformLocation(this->ProgramId, name.c_str());
    glUniform1i(uniformLocation, value);
}

auto Shader::SetFloatUniform(CONST std::string& name, FLOAT value) -> VOID
{
    GLint uniformLocation = glGetUniformLocation(this->ProgramId, name.c_str());
    glUniform1f(uniformLocation, value);
}

auto Shader::SetVector2Uniform(CONST std::string& name, CONST glm::vec2& value) -> VOID
{
    GLint uniformLocation = glGetUniformLocation(this->ProgramId, name.c_str());
    glUniform2fv(uniformLocation, 1, &value[0]);
}

auto Shader::SetVector2Uniform(CONST std::string& name, FLOAT x, FLOAT y) -> VOID
{
    GLint uniformLocation = glGetUniformLocation(this->ProgramId, name.c_str());
    glUniform2f(uniformLocation, x, y);
}

auto Shader::SetVector3Uniform(CONST std::string& name, CONST glm::vec3& value) -> VOID
{
    GLint uniformLocation = glGetUniformLocation(this->ProgramId, name.c_str());
    glUniform3fv(uniformLocation, 1, &value[0]);
}

auto Shader::SetVector3Uniform(CONST std::string& name, FLOAT x, FLOAT y, FLOAT z) -> VOID
{
    GLint uniformLocation = glGetUniformLocation(this->ProgramId, name.c_str());
    glUniform3f(uniformLocation, x, y, z);
}

auto Shader::SetVector4Uniform(CONST std::string& name, CONST glm::vec4& value) -> VOID
{
    GLint uniformLocation = glGetUniformLocation(this->ProgramId, name.c_str());
    glUniform4fv(uniformLocation, 1, &value[0]);
}

auto Shader::SetVector4Uniform(CONST std::string& name, FLOAT x, FLOAT y, FLOAT z, FLOAT w) -> VOID
{
    GLint uniformLocation = glGetUniformLocation(this->ProgramId, name.c_str());
    glUniform4f(uniformLocation, x, y, z, w);
}

auto Shader::SetMatrix2Uniform(CONST std::string& name, CONST glm::mat2& value) -> VOID
{
    GLint uniformLocation = glGetUniformLocation(this->ProgramId, name.c_str());
    glUniformMatrix2fv(uniformLocation, 1, GL_FALSE, &value[0][0]);
}

auto Shader::SetMatrix3Uniform(CONST std::string& name, CONST glm::mat3& value) -> VOID
{
    GLint uniformLocation = glGetUniformLocation(this->ProgramId, name.c_str());
    glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, &value[0][0]);
}

auto Shader::SetMatrix4Uniform(CONST std::string& name, CONST glm::mat4& value) -> VOID
{
    GLint uniformLocation = glGetUniformLocation(this->ProgramId, name.c_str());
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &value[0][0]);
}

auto Shader::CheckCompileErrors(GLuint shaderId, CONST std::string& type) -> BOOL
{
    GLint isSuccess;

    if(type != "PROGRAM")
    {
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isSuccess);
        if(!isSuccess)
        {
            glGetShaderInfoLog(shaderId, SHADER_LOG_SIZE, nullptr, this->ShaderLog);
        }
    }
    else
    {
        glGetProgramiv(shaderId, GL_LINK_STATUS, &isSuccess);
        if(!isSuccess)
        {
            glGetProgramInfoLog(shaderId, SHADER_LOG_SIZE, nullptr, this->ShaderLog);
        }
    }

    return isSuccess;
}

auto Shader::LoadFileFromResource(INT resourceId, UINT& size, PCSTR& data) -> BOOL
{
    HMODULE moduleHandle = ::GetModuleHandle(nullptr);
    HRSRC resourceHandle = ::FindResource(moduleHandle, MAKEINTRESOURCE(resourceId), "TEXT"); 

    if(!resourceHandle)
        return FALSE;

    HGLOBAL resourceData = ::LoadResource(moduleHandle, resourceHandle);

    if(!resourceData)
        return FALSE;

    size = ::SizeofResource(moduleHandle, resourceHandle);
    data = static_cast<PCSTR>(::LockResource(resourceData));

    return TRUE;
}