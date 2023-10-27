#include "Quaternion.h"

#include <cmath>

const Quaternion Quaternion::identity{ 0.0f,0.0f,0.0f,1.0f };

Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs) {
    return {
        lhs.y * rhs.z - lhs.z * rhs.y + lhs.x * rhs.w + lhs.w * rhs.x,
        lhs.z * rhs.x - lhs.x * rhs.z + lhs.y * rhs.w + lhs.w * rhs.y,
        lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w + lhs.w * rhs.z,
        lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
    };
}

Matrix4x4 MakeRotateMatrix(const Quaternion& rotate) {
    float w2 = rotate.w * rotate.w, x2 = rotate.x * rotate.x, y2 = rotate.y * rotate.y, z2 = rotate.z * rotate.z;
    float wx = rotate.w * rotate.x, wy = rotate.w * rotate.y, wz = rotate.w * rotate.z;
    float xy = rotate.x * rotate.y, xz = rotate.x * rotate.z, yz = rotate.y * rotate.z;
    return {
        w2 + x2 - y2 - z2,	2.0f * (wz + xy),	2.0f * (xz - wy),	0.0f,
        2.0f * (xy - wz),	w2 - x2 + y2 - z2,	2.0f * (yz + wx),	0.0f,
        2.0f * (wy + xz),	2.0f * (-wx + yz),	w2 - x2 - y2 + z2,	0.0f,
        0.0f,				0.0f,				0.0f,				1.0f };
}

Quaternion Quaternion::MakeFromAxisAngle(const Vector3& axis, float angle) {
    Vector3 v = Normalize(axis) * std::sin(angle * 0.5f);
    return {
        v.x,
        v.y,
        v.z,
        std::cos(angle * 0.5f) };
}

Quaternion Quaternion::Conjugate() const {
    return { -x, -y, -z, w };
}

float Quaternion::Norm() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}

Quaternion Quaternion::Normalized() const {
    float normInv = 1.0f / Norm();
    return { x * normInv, y * normInv, z * normInv, w * normInv };
}

Quaternion Quaternion::Inverse() const {
    float normSqInv = 1.0f / (x * x + y * y + z * z + w * w);
    return { -x * normSqInv, -y * normSqInv, -z * normSqInv, w * normSqInv };
}

Vector3 Quaternion::ApplyRotate(const Vector3& vector) {
    float a = w * vector.x + y * vector.z - z * vector.y;
    float b = w * vector.y + z * vector.x - x * vector.z;
    float c = w * vector.z + x * vector.y - y * vector.x;
    float d = -x * vector.x - y * vector.y - z * vector.z;

    return {
        a * w + d * -x + b * -z - c * -y,
        b * w + d * -y + c * -x - a * -z,
        c * w + d * -z + a * -y - b * -x,
    };
}
