#pragma once

#include <cstdint>

#include "Vector4.h"

namespace Math {

    // 内部的にはRGBAのfloat型 \
    // floatの場合 : 0~1 \
    // uintの場合 : 0~1 
    class Color {
    public:
        static const Color white;
        static const Color black;
        static const Color red;
        static const Color green;
        static const Color blue;
        static const Color yellow;
        static const Color aqua;
        static const Color purpure;
        static const Color gray;

        static Color HSVA(float h, float s, float v, float a = 1.0f);
        static Color RGBA(float r, float g, float b, float a = 1.0f);
        static uint32_t Merge(uint32_t r, uint32_t g, uint32_t b, uint32_t a);
        static uint32_t Convert(const Vector4& rgba);
        static Vector4 Convert(uint32_t rgba);

        Color() : rgba_(Vector4::one) {}
        Color(float r, float g, float b, float a = 1.0f) : rgba_(r, g, b, a) {}
        Color(uint32_t r, uint32_t g, uint32_t b, uint32_t a = 0xFFu) : rgba_(Convert(Merge(r, g, b, a))) {}
        explicit Color(uint32_t rgba) : rgba_(Convert(rgba)) {}

        operator const Vector4& () noexcept { return rgba_; }
        operator uint32_t() noexcept { return Convert(rgba_); }

        bool operator==(const Color& other) { return rgba_ == other.rgba_; };
        bool operator!=(const Color& other) { return rgba_ != other.rgba_; };

        void SetR(float r) { rgba_.x = r; }
        void SetG(float g) { rgba_.y = g; }
        void SetB(float b) { rgba_.z = b; }
        void SetA(float a) { rgba_.w = a; }

        void SetR(uint32_t r);
        void SetG(uint32_t g);
        void SetB(uint32_t b);
        void SetA(uint32_t a);

        float GetR() const { return rgba_.x; }
        float GetG() const { return rgba_.y; }
        float GetB() const { return rgba_.z; }
        float GetA() const { return rgba_.w; }

    private:
        Vector4 rgba_;
    };

}