#pragma once

struct Quaternion {
    static const Quaternion identity;

    float x;
    float y;
    float z;
    float w;

    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    friend Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);

    Quaternion Conjugate() const;
    float Norm() const;
    Quaternion Normalized() const;
    Quaternion Inverse() const;
};
