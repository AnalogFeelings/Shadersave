#pragma once

#include <windows.h>
#include <GL/glew.h>
#include <glm.hpp>
#include <string>
#include <sstream>
#include <regex>
#include <memory>

#define SHADER_LOG_SIZE 1024

class Shader
{
public:
    CHAR ShaderLog[SHADER_LOG_SIZE];

    auto LoadShader(CONST std::string& vertexText) -> BOOL;
    auto LoadShadertoyShader(std::string& fragmentText) -> BOOL;
    auto CreateProgram() -> BOOL;
    auto UseShader() -> VOID;

    auto SetBoolUniform(CONST std::string& name, BOOL value) -> VOID;
    auto SetIntUniform(CONST std::string& name, INT value) -> VOID;
    auto SetFloatUniform(CONST std::string& name, FLOAT value) -> VOID;
    auto SetVector2Uniform(CONST std::string& name, CONST glm::vec2& value) -> VOID;
    auto SetVector2Uniform(CONST std::string& name, FLOAT x, FLOAT y) -> VOID;
    auto SetVector3Uniform(CONST std::string& name, CONST glm::vec3& value) -> VOID;
    auto SetVector3Uniform(CONST std::string& name, FLOAT x, FLOAT y, FLOAT z) -> VOID;
    auto SetVector4Uniform(CONST std::string& name, CONST glm::vec4& value) -> VOID;
    auto SetVector4Uniform(CONST std::string& name, FLOAT x, FLOAT y, FLOAT z, FLOAT w) -> VOID;
    auto SetMatrix2Uniform(CONST std::string& name, CONST glm::mat2& value) -> VOID;
    auto SetMatrix3Uniform(CONST std::string& name, CONST glm::mat3& value) -> VOID;
    auto SetMatrix4Uniform(CONST std::string& name, CONST glm::mat4& value) -> VOID;
private:
    UINT ProgramId;
    UINT VertexShader;
    UINT FragmentShader;

    auto CheckCompileErrors(GLuint shaderId, CONST std::string& type) -> BOOL;
    auto LoadFileFromResource(INT resourceId, UINT& size, PCSTR& data) -> BOOL;
};