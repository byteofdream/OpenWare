#pragma once

// Renderer shader module: compiles GLSL programs and exposes uniform helpers.

#include <string>

#include "Engine/Core/Math.hpp"

namespace ow {

class Shader {
public:
    Shader() = default;
    ~Shader();

    bool Compile(const std::string& vertexSource, const std::string& fragmentSource);
    void Use() const;

    void SetMat4(const char* name, const Mat4& value) const;
    void SetVec2(const char* name, const Vec2& value) const;
    void SetVec3(const char* name, const Vec3& value) const;
    void SetInt(const char* name, int value) const;
    void SetFloat(const char* name, float value) const;

    unsigned int Id() const { return programId_; }

private:
    unsigned int programId_ = 0;
};

} // namespace ow
