#pragma once

namespace Math {

    struct Vector4 {
        static const Vector4 zero;
        static const Vector4 one;
        static const Vector4 unitX;
        static const Vector4 unitY;
        static const Vector4 unitZ;
        static const Vector4 unitW;

        Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
        Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        inline constexpr bool operator==(const Vector4& rhs) noexcept { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
        inline constexpr bool operator!=(const Vector4& rhs) noexcept { return !(*this == rhs); }

        float x, y, z, w;
    };

}