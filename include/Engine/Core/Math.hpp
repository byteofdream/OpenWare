#pragma once

// Core math module: lightweight vector and matrix types used by all engine systems.

#include <cmath>
#include <cstddef>

namespace ow {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vec3 operator+(const Vec3& rhs) const { return Vec3{x + rhs.x, y + rhs.y, z + rhs.z}; }
    Vec3 operator-(const Vec3& rhs) const { return Vec3{x - rhs.x, y - rhs.y, z - rhs.z}; }
    Vec3 operator*(float scalar) const { return Vec3{x * scalar, y * scalar, z * scalar}; }
    Vec3 operator/(float scalar) const { return Vec3{x / scalar, y / scalar, z / scalar}; }

    Vec3& operator+=(const Vec3& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
};

inline float Dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

inline Vec3 Cross(const Vec3& a, const Vec3& b) {
    return Vec3{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

inline float Length(const Vec3& v) { return std::sqrt(Dot(v, v)); }

inline Vec3 Normalize(const Vec3& v) {
    const float len = Length(v);
    if (len <= 0.00001f) {
        return Vec3{0.0f, 0.0f, 0.0f};
    }
    return v / len;
}

inline float Radians(float degrees) {
    static constexpr float kPi = 3.14159265358979323846f;
    return degrees * (kPi / 180.0f);
}

struct Mat4 {
    float m[16]{};

    float& operator[](std::size_t index) { return m[index]; }
    const float& operator[](std::size_t index) const { return m[index]; }

    static Mat4 Identity() {
        Mat4 result{};
        result[0] = 1.0f;
        result[5] = 1.0f;
        result[10] = 1.0f;
        result[15] = 1.0f;
        return result;
    }

    static Mat4 Translation(const Vec3& v) {
        Mat4 result = Identity();
        result[12] = v.x;
        result[13] = v.y;
        result[14] = v.z;
        return result;
    }

    static Mat4 Scale(const Vec3& v) {
        Mat4 result = Identity();
        result[0] = v.x;
        result[5] = v.y;
        result[10] = v.z;
        return result;
    }

    static Mat4 RotationX(float radians) {
        Mat4 result = Identity();
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        result[5] = c;
        result[6] = s;
        result[9] = -s;
        result[10] = c;
        return result;
    }

    static Mat4 RotationY(float radians) {
        Mat4 result = Identity();
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        result[0] = c;
        result[2] = -s;
        result[8] = s;
        result[10] = c;
        return result;
    }

    static Mat4 RotationZ(float radians) {
        Mat4 result = Identity();
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        result[0] = c;
        result[1] = s;
        result[4] = -s;
        result[5] = c;
        return result;
    }

    static Mat4 Perspective(float fovRadians, float aspect, float nearPlane, float farPlane) {
        Mat4 result{};
        const float tanHalf = std::tan(fovRadians * 0.5f);
        result[0] = 1.0f / (aspect * tanHalf);
        result[5] = 1.0f / tanHalf;
        result[10] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        result[11] = -1.0f;
        result[14] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        return result;
    }

    static Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
        const Vec3 f = Normalize(target - eye);
        const Vec3 s = Normalize(Cross(f, up));
        const Vec3 u = Cross(s, f);

        Mat4 result = Identity();
        result[0] = s.x;
        result[4] = s.y;
        result[8] = s.z;

        result[1] = u.x;
        result[5] = u.y;
        result[9] = u.z;

        result[2] = -f.x;
        result[6] = -f.y;
        result[10] = -f.z;

        result[12] = -Dot(s, eye);
        result[13] = -Dot(u, eye);
        result[14] = Dot(f, eye);
        return result;
    }
};

inline Mat4 operator*(const Mat4& a, const Mat4& b) {
    Mat4 result{};
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            float sum = 0.0f;
            for (int i = 0; i < 4; ++i) {
                sum += a[row + i * 4] * b[i + col * 4];
            }
            result[row + col * 4] = sum;
        }
    }
    return result;
}

} // namespace ow
