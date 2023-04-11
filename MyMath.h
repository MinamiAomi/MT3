#pragma once
#include <cmath>
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix4x4.h>
#include "Novice.h"

namespace Math 
{
	constexpr float Pi = 3.141592653589793f;
	constexpr float TwoPi = Pi * 2.0f;
	constexpr float HalfPi = Pi * 0.5f;
}

inline Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
	return { v1.x + v2.x, v1.y + v2.y };
}
inline Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
	return { v1.x - v2.x, v1.y - v2.y };
}
inline Vector2 operator*(const Vector2& v, float s)
{
	return { v.x * s, v.y * s };
}
inline Vector2 operator*(float s, const Vector2& v)
{
	return { s * v.x, s * v.y };
}
inline Vector2& operator+=(Vector2& v1, const Vector2& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	return v1;
}
inline Vector2& operator-=(Vector2& v1, const Vector2& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	return v1;
}
inline Vector2& operator*=(Vector2& v, float s)
{
	v.x *= s;
	v.y *= s;
	return v;
}
inline bool operator==(const Vector2& v1, const Vector2& v2)
{
	return v1.x == v2.x && v1.y == v2.y;
}
inline bool operator!=(const Vector2& v1, const Vector2& v2)
{
	return v1.x != v2.x || v1.y != v2.y;
}
inline float Dot(const Vector2& v1, const Vector2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}
inline float Cross(const Vector2& v1, const Vector2& v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}
inline float LengthSquare(const Vector2& v)
{
	return Dot(v, v);
}
inline float Length(const Vector2& v)
{
	return std::sqrt(LengthSquare(v));
}
inline Vector2 Normalize(const Vector2& v)
{
	return v * (1.0f / Length(v));
}

inline Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}
inline Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}
inline Vector3 operator*(const Vector3& v, float s)
{
	return { v.x * s, v.y * s, v.z * s };
}
inline Vector3 operator*(float s, const Vector3& v)
{
	return { s * v.x, s * v.y, s * v.z };
}
inline Vector3& operator+=(Vector3& v1, const Vector3& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}
inline Vector3& operator-=(Vector3& v1, const Vector3& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	return v1;
}
inline Vector3& operator*=(Vector3& v, float s)
{
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}
inline bool operator==(const Vector3& v1, const Vector3& v2)
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}
inline bool operator!=(const Vector3& v1, const Vector3& v2)
{
	return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
}
inline float Dot(const Vector3& v1, const Vector3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
inline Vector3 Cross(const Vector3& v1, const Vector3& v2)
{
	return { 
		v1.y * v2.z - v1.z * v2.y, 
		v1.z * v2.x - v1.x * v2.z, 
		v1.x * v2.y - v1.y * v2.x };
}
inline float LengthSquare(const Vector3& v)
{
	return Dot(v, v);
}
inline float Length(const Vector3& v)
{
	return std::sqrt(LengthSquare(v));
}
inline Vector3 Normalize(const Vector3& v)
{
	return v * (1.0f / Length(v));
}

void VectorScreenPrintf(int x, int y, const Vector2& v, const char* label)
{
	constexpr int kCoulumnWidth = 60;
	Novice::ScreenPrintf(x, y, "%.02f", v.x);
	Novice::ScreenPrintf(x + kCoulumnWidth, y, "%.02f", v.y);
	Novice::ScreenPrintf(x + kCoulumnWidth * 2, y, "%s", label);
}
void VectorScreenPrintf(int x, int y, const Vector3& v, const char* label)
{
	constexpr int kCoulumnWidth = 60;
	Novice::ScreenPrintf(x, y, "%.02f", v.x);
	Novice::ScreenPrintf(x + kCoulumnWidth, y, "%.02f", v.y);
	Novice::ScreenPrintf(x + kCoulumnWidth * 2, y, "%.02f", v.z);
	Novice::ScreenPrintf(x + kCoulumnWidth * 3, y, "%s", label);
}