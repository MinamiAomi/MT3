#include "Color.h"

#include "Common.h"

namespace Math {

    const Color Color::white(1.0f, 1.0f, 1.0f, 1.0f);
    const Color Color::black(0.0f, 0.0f, 0.0f, 1.0f);
    const Color Color::red(1.0f, 0.0f, 0.0f, 1.0f);
    const Color Color::green(0.0f, 1.0f, 0.0f, 1.0f);
    const Color Color::blue(0.0f, 0.0f, 1.0f, 1.0f);
    const Color Color::yellow(1.0f, 1.0f, 0.0f, 1.0f);
    const Color Color::aqua(0.0f, 1.0f, 1.0f, 1.0f);
    const Color Color::purpure(1.0f, 0.0f, 1.0f, 1.0f);
    const Color Color::gray(0.5f, 0.5f, 0.5f, 0.5f);


    Color Color::HSVA(float h, float s, float v, float a) {
        float r = v, g = v, b = v;
        if (s > 0.0f) {
            h *= 6.0f;
            int32_t i = int32_t(h);
            float f = h - float(i);
            switch (i)
            {
            default:
            case 0:
                g *= 1.0f - s * (1.0f - f);
                b *= 1.0f - s;
                break;
            case 1:
                r *= 1.0f - s * f;
                b *= 1.0f - s;
                break;
            case 2:
                r *= 1.0f - s;
                b *= 1.0f - s * (1.0f - f);
                break;
            case 3:
                r *= 1.0f - s;
                g *= 1.0f - s * f;
                break;
            case 4:
                r *= 1.0f - s * (1.0f - f);
                g *= 1.0f - s;
                break;
            case 5:
                g *= 1.0f - s;
                b *= 1.0f - s * f;
                break;
            }
        }
        return { r, g, b, a };
    }

    Color Color::RGBA(float r, float g, float b, float a) {
        return { r, g, b, a };
    }

    void Color::SetR(uint32_t r) {
        constexpr float reci = 1.0f / 255.0f;
        rgba_.x = Clamp(float(r) * reci, 0.0f, 1.0f);
    }

    void Color::SetG(uint32_t g) {
        constexpr float reci = 1.0f / 255.0f;
        rgba_.y = Clamp(float(g) * reci, 0.0f, 1.0f);
    }

    void Color::SetB(uint32_t b) {
        constexpr float reci = 1.0f / 255.0f;
        rgba_.z = Clamp(float(b) * reci, 0.0f, 1.0f);
    }

    void Color::SetA(uint32_t a) {
        constexpr float reci = 1.0f / 255.0f;
        rgba_.w = Clamp(float(a) * reci, 0.0f, 1.0f);
    }

    uint32_t Color::Merge(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
        return (r << 24u | g << 16u | b << 8u | a);
    }

    uint32_t Color::Convert(const Vector4& rgba) {
        uint32_t r = Min(uint32_t(rgba.x * 0xFFu), 0xFFu);
        uint32_t g = Min(uint32_t(rgba.y * 0xFFu), 0xFFu);
        uint32_t b = Min(uint32_t(rgba.z * 0xFFu), 0xFFu);
        uint32_t a = Min(uint32_t(rgba.w * 0xFFu), 0xFFu);
        return Merge(r, g, b, a);
    }

    Vector4 Color::Convert(uint32_t rgba) {
        constexpr float reci = 1.0f / 255.0f;
        auto GetElement = [&](uint32_t index) {
            return ((rgba & (0xFFu << index)) >> (8 * index)) * reci;
        };
        Vector4 result;
        result.x = Clamp(GetElement(3), 0.0f, 1.0f);
        result.y = Clamp(GetElement(2), 0.0f, 1.0f);
        result.z = Clamp(GetElement(1), 0.0f, 1.0f);
        result.w = Clamp(GetElement(0), 0.0f, 1.0f);
        return result;
    }

}

