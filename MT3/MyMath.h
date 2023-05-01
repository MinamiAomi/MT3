#pragma once
#include <cmath>
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix4x4.h>
#include <MyMath_inline.h>
#include "Geometry.h"

namespace Math {
	constexpr float Pi = 3.141592653589793f;
	constexpr float TwoPi = Pi * 2.0f;
	constexpr float HalfPi = Pi * 0.5f;

	constexpr inline float ToRad(float deg) { return deg * Pi / 180.0f; }
	constexpr inline float ToDeg(float rad) { return rad * 180.0f / Pi; }
}

static const Vector2 kVector2Zero{ 0.0f,0.0f };
static const Vector2 kVector2UnitX{ 1.0f,0.0f };
static const Vector2 kVector2UnitY{ 0.0f,1.0f };
static const Vector3 kVector3Zero{ 0.0f,0.0f,0.0f };
static const Vector3 kVector3UnitX{ 1.0f,0.0f,0.0f };
static const Vector3 kVector3UnitY{ 0.0f,1.0f,0.0f };
static const Vector3 kVector3UnitZ{ 0.0f,0.0f,1.0f };

inline Vector2 operator+(const Vector2& v);
inline Vector2 operator-(const Vector2& v);
inline Vector2 operator+(const Vector2& v1, const Vector2& v2);
inline Vector2 operator-(const Vector2& v1, const Vector2& v2);
inline Vector2 operator*(const Vector2& v, float s);
inline Vector2 operator*(float s, const Vector2& v);
inline Vector2& operator+=(Vector2& v1, const Vector2& v2);
inline Vector2& operator-=(Vector2& v1, const Vector2& v2);
inline Vector2& operator*=(Vector2& v, float s);
inline bool operator==(const Vector2& v1, const Vector2& v2);
inline bool operator!=(const Vector2& v1, const Vector2& v2);
inline float Dot(const Vector2& v1, const Vector2& v2);
inline float Cross(const Vector2& v1, const Vector2& v2);
inline float LengthSquare(const Vector2& v);
inline float Length(const Vector2& v);
inline Vector2 Normalize(const Vector2& v);
inline Vector2 Project(const Vector2& v1, const Vector2& v2);
inline Vector2 Lerp(const Vector2& start, const Vector2& end, float t);

inline Vector3 operator+(const Vector3& v);
inline Vector3 operator-(const Vector3& v);
inline Vector3 operator+(const Vector3& v1, const Vector3& v2);
inline Vector3 operator-(const Vector3& v1, const Vector3& v2);
inline Vector3 operator*(const Vector3& v, float s);
inline Vector3 operator*(float s, const Vector3& v);
inline Vector3& operator+=(Vector3& v1, const Vector3& v2);
inline Vector3& operator-=(Vector3& v1, const Vector3& v2);
inline Vector3& operator*=(Vector3& v, float s);
inline bool operator==(const Vector3& v1, const Vector3& v2);
inline bool operator!=(const Vector3& v1, const Vector3& v2);
inline float Dot(const Vector3& v1, const Vector3& v2);
inline Vector3 Cross(const Vector3& v1, const Vector3& v2);
inline float LengthSquare(const Vector3& v);
inline float Length(const Vector3& v);
inline Vector3 Normalize(const Vector3& v);
inline Vector3 Lerp(const Vector3& start, const Vector3& end, float t);
inline Vector3 Project(const Vector3& v1, const Vector3& v2);

inline Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2);
inline Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2);
inline Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
inline Matrix4x4 operator*(const Matrix4x4& m, float s);
inline Matrix4x4 operator*(float s, const Matrix4x4& m);
inline Matrix4x4 operator*=(Matrix4x4& m1, const Matrix4x4& m2);
inline Matrix4x4 Transpose(const Matrix4x4& m);
float Determinant(const Matrix4x4& m);
Matrix4x4 Adjugate(const Matrix4x4& m);
Matrix4x4 Inverse(const Matrix4x4& m);
inline Matrix4x4 MakeIdentityMatrix();
inline Matrix4x4 MakeScaleMatrix(const Vector3& scale);
inline Matrix4x4 MakeRotateXMatrix(float rad);
inline Matrix4x4 MakeRotateYMatrix(float rad);
inline Matrix4x4 MakeRotateZMatrix(float rad);
inline Matrix4x4 MakeTranslateMatrix(const Vector3& trans);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& trans);
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspect, float nearZ, float farZ);
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearZ, float farZ);
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

Vector3 Transform(const Vector3& v, const Matrix4x4& m);
inline Vector3 operator*(const Vector3& v, const Matrix4x4& m);


void VectorScreenPrintf(int x, int y, const Vector2& v, const char* label);
void VectorScreenPrintf(int x, int y, const Vector3& v, const char* label);
void MatrixScreenPrintf(int x, int y, const Matrix4x4& m, const char* label);

Vector3 ClosestPoint(const Vector3& point, const Line& line);
Vector3 ClosestPoint(const Vector3& point, const Ray& ray);
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);