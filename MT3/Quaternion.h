#pragma once
#include "MyMath.h"

struct Quaternion {
    static const Quaternion identity;

    static Quaternion MakeFromAxisAngle(const Vector3& axis, float angle);

    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    friend Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs);
    friend Quaternion operator*(float lhs, const Quaternion& rhs);
    friend Quaternion operator*(const Quaternion& lhs, float rhs);
    friend Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);

    friend Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
    
    Quaternion Conjugate() const;
    float Norm() const;
    Quaternion Normalized() const;
    Quaternion Inverse() const;
    Vector3 ApplyRotate(const Vector3& vector);

    float x;
    float y;
    float z;
    float w;
};

Matrix4x4 MakeRotateMatrix(const Quaternion& rotate);