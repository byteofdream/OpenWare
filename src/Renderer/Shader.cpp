#include "Engine/Renderer/Shader.hpp"

#include "Engine/Renderer/GL.hpp"

#include <iostream>

namespace ow {

namespace {

bool CheckShader(unsigned int shader, const char* stageName) {
    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_TRUE) {
        return true;
    }

    char infoLog[1024]{};
    glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
    std::cerr << "Shader compile error (" << stageName << "): " << infoLog << '\n';
    return false;
}

bool CheckProgram(unsigned int program) {
    int success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_TRUE) {
        return true;
    }

    char infoLog[1024]{};
    glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
    std::cerr << "Shader link error: " << infoLog << '\n';
    return false;
}

} // namespace

Shader::~Shader() {
    if (programId_ != 0) {
        glDeleteProgram(programId_);
    }
}

bool Shader::Compile(const std::string& vertexSource, const std::string& fragmentSource) {
    const char* vertexSrc = vertexSource.c_str();
    const char* fragmentSrc = fragmentSource.c_str();

    const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
    glCompileShader(vertexShader);
    if (!CheckShader(vertexShader, "vertex")) {
        glDeleteShader(vertexShader);
        return false;
    }

    const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
    glCompileShader(fragmentShader);
    if (!CheckShader(fragmentShader, "fragment")) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    const unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!CheckProgram(program)) {
        glDeleteProgram(program);
        return false;
    }

    if (programId_ != 0) {
        glDeleteProgram(programId_);
    }
    programId_ = program;
    return true;
}

void Shader::Use() const {
    glUseProgram(programId_);
}

void Shader::SetMat4(const char* name, const Mat4& value) const {
    const int location = glGetUniformLocation(programId_, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, value.m);
}

void Shader::SetVec3(const char* name, const Vec3& value) const {
    const int location = glGetUniformLocation(programId_, name);
    glUniform3f(location, value.x, value.y, value.z);
}

void Shader::SetInt(const char* name, int value) const {
    const int location = glGetUniformLocation(programId_, name);
    glUniform1i(location, value);
}

void Shader::SetFloat(const char* name, float value) const {
    const int location = glGetUniformLocation(programId_, name);
    glUniform1f(location, value);
}

} // namespace ow
