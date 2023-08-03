#pragma once

#include "Common.h"

namespace Math {

    struct Vector2 {
        static const Vector2 zero;
        static const Vector2 one;
        static const Vector2 unitX;
        static const Vector2 unitY;
        static const Vector2 right;
        static const Vector2 left;
        static const Vector2 up;
        static const Vector2 down;

        constexpr Vector2() noexcept : x(0.0f), y(0.0f) {}
        constexpr Vector2(float x, float y) noexcept : x(x), y(y) {}
        explicit constexpr Vector2(float xy) noexcept : x(xy), y(xy) {}

        inline float& operator[](size_t i) { return reinterpret_cast<float*>(this)[i]; }
        inline const float& operator[](size_t i) const { return reinterpret_cast<const float*>(this)[i]; }

        friend inline constexpr bool operator==(const Vector2& a, const Vector2& b) noexcept { return a.x == b.x && a.y == b.y; }
        friend inline constexpr bool operator!=(const Vector2& a, const Vector2& b) noexcept { return !(a == b); }

        friend inline constexpr Vector2 operator+(const Vector2& a) noexcept { return { a.x, a.y }; }
        friend inline constexpr Vector2 operator-(const Vector2& a) noexcept { return { -a.x, -a.y }; }

        friend inline constexpr Vector2 operator+(const Vector2& a, const Vector2& b) noexcept { return { a.x + b.x, a.y + b.y }; }
        friend inline constexpr Vector2 operator-(const Vector2& a, const Vector2& b) noexcept { return { a.x - b.x, a.y - b.y }; }
        friend inline constexpr Vector2 operator*(const Vector2& a, float b) noexcept { return { a.x * b, a.y * b }; }
        friend inline constexpr Vector2 operator*(float a, const Vector2& b) noexcept { return b * a; }
        friend inline constexpr Vector2 operator/(const Vector2& a, float b) noexcept { return a * (1.0f / b); }

        inline constexpr Vector2& operator+=(const Vector2& a) noexcept { *this = *this + a; return *this; }
        inline constexpr Vector2& operator-=(const Vector2& a) noexcept { *this = *this - a; return *this; }
        inline constexpr Vector2& operator*=(float a) noexcept { *this = *this * a; return *this; }
        inline constexpr Vector2& operator/=(float a) noexcept { *this = *this / a; return *this; }

        inline constexpr float Dot(const Vector2& a) const noexcept { return x * a.x + y * a.y; }
        inline constexpr float Cross(const Vector2& a) const noexcept { return x * a.y - y * a.x; }
        inline constexpr Vector2 HadamardProduct(const Vector2& a) const noexcept { return { x * a.x, y * a.y }; }
        inline constexpr Vector2 TripleProduct(const Vector2& a, const Vector2& b) const noexcept { return Dot(b) * a - Dot(a) * b; }
        inline constexpr float LengthSquare() const noexcept { return x * x + y * y; }
        inline float Length() const noexcept { return Sqrt(LengthSquare()); }
        inline constexpr Vector2 Normalized() const noexcept { return *this / Length(); }

        inline constexpr Vector2 NormalProject(const Vector2& a) const noexcept { return Dot(a) * a; }
        inline constexpr Vector2 Project(const Vector2& a) const noexcept { return NormalProject(a.Normalized()); }
        inline constexpr Vector2 Reflect(const Vector2& a) const noexcept { return *this - 2.0f * NormalProject(a.Normalized()); }
        inline float Distance(const Vector2& a) const noexcept { return (*this - a).Length(); }

        inline float Angle(const Vector2& a) const noexcept { return Acos(Normalized().Dot(a.Normalized())); }
        inline float SignedAngle(const Vector2& a) const noexcept { return (Cross(a) >= 0.0f) ? Angle(a) : -Angle(a); }

        inline constexpr bool IsEqualDirection(const Vector2& a) const noexcept { return Dot(a) > 1.0f - Epsilon; }
        inline constexpr bool IsZero() const noexcept { return *this == zero; }

        friend constexpr Vector2 Lerp(float t, const Vector2& a, const Vector2& b) { return a + t * (b - a); }
        friend constexpr Vector2 Min(const Vector2& a, const Vector2& b) { return { Min(a.x,b.x), Min(a.y,b.y) }; }
        friend constexpr Vector2 Max(const Vector2& a, const Vector2& b) { return { Max(a.x,b.x), Max(a.y,b.y) }; }
        friend constexpr Vector2 Max(const Vector2& val, const Vector2& min, const Vector2& max) { return { Clamp(val.x, min.x, max.x), Clamp(val.y, min.y, max.y) }; }
  
        float x, y;
    };

}