#pragma once

#include <cmath>
#include <cstdint>
#include <limits>

namespace Math {

    constexpr int8_t Int8Min = std::numeric_limits<int8_t>::min();
    constexpr int8_t Int8Max = std::numeric_limits<int8_t>::max();
    constexpr int16_t Int16Min = std::numeric_limits<int16_t>::min();
    constexpr int16_t Int16Max = std::numeric_limits<int16_t>::max();
    constexpr int32_t Int32Min = std::numeric_limits<int32_t>::min();
    constexpr int32_t Int32Max = std::numeric_limits<int32_t>::max();
    constexpr int64_t Int64Min = std::numeric_limits<int64_t>::min();
    constexpr int64_t Int64Max = std::numeric_limits<int64_t>::max();
    constexpr uint8_t UInt8Min = std::numeric_limits<uint8_t>::min();
    constexpr uint8_t UInt8Max = std::numeric_limits<uint8_t>::max();
    constexpr uint16_t UInt16Min = std::numeric_limits<uint16_t>::min();
    constexpr uint16_t UInt16Max = std::numeric_limits<uint16_t>::max();
    constexpr uint32_t UInt32Min = std::numeric_limits<uint32_t>::min();
    constexpr uint32_t UInt32Max = std::numeric_limits<uint32_t>::max();
    constexpr uint64_t UInt64Min = std::numeric_limits<uint64_t>::min();
    constexpr uint64_t UInt64Max = std::numeric_limits<uint64_t>::max();
    constexpr float FloatMax = std::numeric_limits<float>::max();
    constexpr float FloatMin = std::numeric_limits<float>::min();

    constexpr float Epsilon = 0.00001f;

    /*   constexpr float Pi = 3.141592653589793f;
       constexpr float TwoPi = Pi * 2.0f;
       constexpr float HalfPi = Pi * 0.5f;
       constexpr float ToRad = Pi / 180.0f;
       constexpr float ToDeg = 180.0f / Pi;*/

#define CREATE_MATH_COMPARE(Type) \
    constexpr inline Type Min(Type lhs, Type rhs) noexcept { return lhs < rhs ? lhs : rhs; }\
    constexpr inline Type Max(Type lhs, Type rhs) noexcept { return lhs > rhs ? lhs : rhs; }\
    constexpr inline Type Clamp(Type val, Type min, Type max) noexcept { return Min(Max(val, min), max); }

    CREATE_MATH_COMPARE(float)
        CREATE_MATH_COMPARE(int8_t)
        CREATE_MATH_COMPARE(int16_t)
        CREATE_MATH_COMPARE(int32_t)
        CREATE_MATH_COMPARE(int64_t)
        CREATE_MATH_COMPARE(uint8_t)
        CREATE_MATH_COMPARE(uint16_t)
        CREATE_MATH_COMPARE(uint32_t)
        CREATE_MATH_COMPARE(uint64_t)

#undef CREATE_MATH_COMPARE

#define CREATE_MATH_ABS(Type)\
    constexpr inline Type Abs(Type x) noexcept { return x < 0 ? -x : x; }

        CREATE_MATH_ABS(float)
        CREATE_MATH_ABS(int8_t)
        CREATE_MATH_ABS(int16_t)
        CREATE_MATH_ABS(int32_t)
        CREATE_MATH_ABS(int64_t)

#undef CREATE_MATH_ABS

    constexpr inline float Lerp(float t, float s, float e) noexcept { return s + t * (e - s); }
    inline float Pow(float x, float y) noexcept { return std::pow(x, y); }
    inline float Sqrt(float x) noexcept { return std::sqrt(x); }
    inline float Sin(float x) noexcept { return std::sin(x); }
    inline float Cos(float x) noexcept { return std::cos(x); }
    inline float Tan(float x) noexcept { return std::tan(x); }
    inline float Asin(float x) noexcept { return std::asin(x); }
    inline float Acos(float x) noexcept { return std::acos(x); }
    inline float Atan(float x) noexcept { return std::atan(x); }
    inline float Atan2(float y, float x) noexcept { return std::atan2(y, x); }
    inline float Sec(float x) noexcept { return 1.0f / std::sin(x); }
    inline float Cosec(float x) noexcept { return 1.0f / std::cos(x); }
    inline float Cot(float x) noexcept { return 1.0f / std::tan(x); }
    inline float Floor(float x) noexcept { return std::floor(x); }
    inline float Ceil(float x) noexcept { return std::ceil(x); }
    inline float Round(float x) noexcept { return std::round(x); }
    inline float Mod(float x, float y) noexcept { return std::fmod(x, y); }

}