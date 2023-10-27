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
    return { -x * normSqInv, -y* normSqInv, -z * normSqInv, w * normSqInv };
}
