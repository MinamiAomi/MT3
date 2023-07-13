#pragma once
#include <cmath>
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix4x4.h>
#include "Geometry.h"

namespace Math {
	constexpr float Pi = 3.141592653589793f;
	constexpr float TwoPi = Pi * 2.0f;
	constexpr float HalfPi = Pi * 0.5f;

	constexpr float ToRadian = Pi / 180.0f;
	constexpr float ToDegree = 180.0f / Pi;

	inline float Lerp(float s, float e, float t) {
		return s + t * (e - s);
	}
}

static const Vector2 Vector2Zero{ 0.0f,0.0f };
static const Vector2 Vector2One{ 1.0f,1.0f };
static const Vector2 Vector2UnitX{ 1.0f,0.0f };
static const Vector2 Vector2UnitY{ 0.0f,1.0f };
static const Vector3 Vector3Zero{ 0.0f,0.0f,0.0f };
static const Vector3 Vector3One{ 1.0f,1.0f,1.0f };
static const Vector3 Vector3UnitX{ 1.0f,0.0f,0.0f };
static const Vector3 Vector3UnitY{ 0.0f,1.0f,0.0f };
static const Vector3 Vector3UnitZ{ 0.0f,0.0f,1.0f };

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
inline Matrix4x4 MakeInverseMatrix(const Vector3& rotate, const Vector3& translate);
inline Matrix4x4 MakeInverseMatrix(const Matrix4x4& rotate, const Vector3& translate);
inline Matrix4x4 MakeIdentityMatrix();
inline Matrix4x4 MakeScaleMatrix(const Vector3& scale);
inline Matrix4x4 MakeRotateXMatrix(float rad);
inline Matrix4x4 MakeRotateYMatrix(float rad);
inline Matrix4x4 MakeRotateZMatrix(float rad);
inline Matrix4x4 MakeRotateXYZMatrix(const Vector3& rotate);
inline Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
inline Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspect, float nearZ, float farZ);
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearZ, float farZ);
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

inline Vector3 GetXAxis(const Matrix4x4& m);
inline Vector3 GetYAxis(const Matrix4x4& m);
inline Vector3 GetZAxis(const Matrix4x4& m);
inline Vector3 GetTranslate(const Matrix4x4& m);

inline Matrix4x4& SetXAxis(Matrix4x4& m, const Vector3& v);
inline Matrix4x4& SetYAxis(Matrix4x4& m, const Vector3& v);
inline Matrix4x4& SetZAxis(Matrix4x4& m, const Vector3& v);
inline Matrix4x4& SetTranslate(Matrix4x4& m, const Vector3& v);

inline void GetOrientations(const Matrix4x4& m, Vector3 orientations[3]);
inline Matrix4x4 MakeRotateMatrixFromOrientations(const Vector3 orientations[3]);

Vector3 Transform(const Vector3& v, const Matrix4x4& m);
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);
inline Vector3 operator*(const Vector3& v, const Matrix4x4& m);

Vector3 CatmullRomSpline(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& controlPoint3, float t);

#include <MyMath_inline.h>