#include "MyMath.h"
#pragma once

inline Vector2 operator+(const Vector2& v) {
	return { +v.x,+v.y };
}
inline Vector2 operator-(const Vector2& v) {
	return { -v.x,-v.y };
}
inline Vector2 operator+(const Vector2& v1, const Vector2& v2) {
	return { v1.x + v2.x, v1.y + v2.y };
}
inline Vector2 operator-(const Vector2& v1, const Vector2& v2) {
	return { v1.x - v2.x, v1.y - v2.y };
}
inline Vector2 operator*(const Vector2& v, float s) {
	return { v.x * s, v.y * s };
}
inline Vector2 operator*(float s, const Vector2& v) {
	return { s * v.x, s * v.y };
}
inline Vector2& operator+=(Vector2& v1, const Vector2& v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	return v1;
}
inline Vector2& operator-=(Vector2& v1, const Vector2& v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	return v1;
}
inline Vector2& operator*=(Vector2& v, float s) {
	v.x *= s;
	v.y *= s;
	return v;
}
inline bool operator==(const Vector2& v1, const Vector2& v2) {
	return v1.x == v2.x && v1.y == v2.y;
}
inline bool operator!=(const Vector2& v1, const Vector2& v2) {
	return v1.x != v2.x || v1.y != v2.y;
}
inline float Dot(const Vector2& v1, const Vector2& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}
inline float Cross(const Vector2& v1, const Vector2& v2) {
	return v1.x * v2.y - v1.y * v2.x;
}
inline float LengthSquare(const Vector2& v) {
	return Dot(v, v);
}
inline float Length(const Vector2& v) {
	return std::sqrt(LengthSquare(v));
}
inline Vector2 Normalize(const Vector2& v) {
	return v * (1.0f / Length(v));
}

inline Vector2 Project(const Vector2& v1, const Vector2& v2) {
	Vector2 normV2 = Normalize(v2);
	return Dot(v1, normV2) * normV2;
}

inline Vector2 Lerp(const Vector2& start, const Vector2& end, float t) {
	return start + t * (end - start);
}

inline Vector3 operator+(const Vector3& v) {
	return { +v.x, +v.y, +v.z };
}
inline Vector3 operator-(const Vector3& v) {
	return { -v.x, -v.y, -v.z };
}
inline Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}
inline Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}
inline Vector3 operator*(const Vector3& v, float s) {
	return { v.x * s, v.y * s, v.z * s };
}
inline Vector3 operator*(float s, const Vector3& v) {
	return { s * v.x, s * v.y, s * v.z };
}
inline Vector3& operator+=(Vector3& v1, const Vector3& v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}
inline Vector3& operator-=(Vector3& v1, const Vector3& v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	return v1;
}
inline Vector3& operator*=(Vector3& v, float s) {
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}
inline bool operator==(const Vector3& v1, const Vector3& v2) {
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}
inline bool operator!=(const Vector3& v1, const Vector3& v2) {
	return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
}
inline float Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
inline Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x };
}
inline float LengthSquare(const Vector3& v) {
	return Dot(v, v);
}
inline float Length(const Vector3& v) {
	return std::sqrt(LengthSquare(v));
}
inline Vector3 Normalize(const Vector3& v) {
	return v * (1.0f / Length(v));
}

inline Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
	return start + t * (end - start);
}

inline Vector3 Project(const Vector3& v1, const Vector3& v2) {
	Vector3 normV2 = Normalize(v2);
	return Dot(v1, normV2) * normV2;
}

inline Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) {
	return {
		m1.m[0][0] + m2.m[0][0], m1.m[0][1] + m2.m[0][1], m1.m[0][2] + m2.m[0][2], m1.m[0][3] + m2.m[0][3],
		m1.m[1][0] + m2.m[1][0], m1.m[1][1] + m2.m[1][1], m1.m[1][2] + m2.m[1][2], m1.m[1][3] + m2.m[1][3],
		m1.m[2][0] + m2.m[2][0], m1.m[2][1] + m2.m[2][1], m1.m[2][2] + m2.m[2][2], m1.m[2][3] + m2.m[2][3],
		m1.m[3][0] + m2.m[3][0], m1.m[3][1] + m2.m[3][1], m1.m[3][2] + m2.m[3][2], m1.m[3][3] + m2.m[3][3] };
}
inline Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) {
	return {
		m1.m[0][0] - m2.m[0][0], m1.m[0][1] - m2.m[0][1], m1.m[0][2] - m2.m[0][2], m1.m[0][3] - m2.m[0][3],
		m1.m[1][0] - m2.m[1][0], m1.m[1][1] - m2.m[1][1], m1.m[1][2] - m2.m[1][2], m1.m[1][3] - m2.m[1][3],
		m1.m[2][0] - m2.m[2][0], m1.m[2][1] - m2.m[2][1], m1.m[2][2] - m2.m[2][2], m1.m[2][3] - m2.m[2][3],
		m1.m[3][0] - m2.m[3][0], m1.m[3][1] - m2.m[3][1], m1.m[3][2] - m2.m[3][2], m1.m[3][3] - m2.m[3][3] };
}
inline Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
	return {
		m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0],
		m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1],
		m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2],
		m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3],

		m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0],
		m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1],
		m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2],
		m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3],

		m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0],
		m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1],
		m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2],
		m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3],

		m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0],
		m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1],
		m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2],
		m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3] };
}
inline Matrix4x4 operator*(const Matrix4x4& m, float s) {
	return {
		m.m[0][0] * s, m.m[0][1] * s, m.m[0][2] * s, m.m[0][3] * s,
		m.m[1][0] * s, m.m[1][1] * s, m.m[1][2] * s, m.m[1][3] * s,
		m.m[2][0] * s, m.m[2][1] * s, m.m[2][2] * s, m.m[2][3] * s,
		m.m[3][0] * s, m.m[3][1] * s, m.m[3][2] * s, m.m[3][3] * s };
}
inline Matrix4x4 operator*(float s, const Matrix4x4& m) {
	return {
		s * m.m[0][0], s * m.m[0][1], s * m.m[0][2], s * m.m[0][3],
		s * m.m[1][0], s * m.m[1][1], s * m.m[1][2], s * m.m[1][3],
		s * m.m[2][0], s * m.m[2][1], s * m.m[2][2], s * m.m[2][3],
		s * m.m[3][0], s * m.m[3][1], s * m.m[3][2], s * m.m[3][3] };
}
inline Matrix4x4 operator*=(Matrix4x4& m1, const Matrix4x4& m2) {
	m1 = m1 * m2;
	return m1;
}
inline Matrix4x4 Transpose(const Matrix4x4& m) {
	return {
		m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
		m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
		m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
		m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3] };
}
inline Matrix4x4 MakeIdentityMatrix() {
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
}
inline Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	return {
		scale.x,	0.0f,		0.0f,		0.0f,
		0.0f,		scale.y,	0.0f,		0.0f,
		0.0f,		0.0f,		scale.z,	0.0f,
		0.0f,		0.0f,		0.0f,		1.0f };
}
inline Matrix4x4 MakeRotateXMatrix(float rotate) {
	float s = std::sin(rotate);
	float c = std::cos(rotate);
	return {
		1.0f,	0.0f,	0.0f,	0.0f,
		0.0f,	c,		s,		0.0f,
		0.0f,	-s,		c,		0.0f,
		0.0f,	0.0f,	0.0f,	1.0f };
}
inline Matrix4x4 MakeRotateYMatrix(float rotate) {
	float s = std::sin(rotate);
	float c = std::cos(rotate);
	return {
		c,		0.0f,	-s,		0.0f,
		0.0f,	1.0f,	0.0f,	0.0f,
		s,		0.0f,	c,		0.0f,
		0.0f,	0.0f,	0.0f,	1.0f };
}
inline Matrix4x4 MakeRotateZMatrix(float rotate) {
	float s = std::sin(rotate);
	float c = std::cos(rotate);
	return {
		c,		s,		0.0f,	0.0f,
		-s,		c,		0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f };
}
inline Matrix4x4 MakeRotateXYZMatrix(const Vector3& rotate) {
	Vector3 s = { std::sin(rotate.x), std::sin(rotate.y), std::sin(rotate.z) };
	Vector3 c = { std::cos(rotate.x), std::cos(rotate.y), std::cos(rotate.z) };
	return {
		c.y * c.z,						c.y * s.z,						-s.y,		0.0f,
		s.x * s.y * c.z - c.x * s.z,	s.x * s.y * s.z + c.x * c.z,	s.x * c.y,	0.0f,
		c.x * s.y * c.z + s.x * s.z,	c.x * s.y * s.z - s.x * c.z,	c.x * c.y,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f };
}
inline Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	return {
		1.0f,			0.0f,			0.0f,			0.0f,
		0.0f,			1.0f,			0.0f,			0.0f,
		0.0f,			0.0f,			1.0f,			0.0f,
		translate.x,	translate.y,	translate.z,	1.0f };
}

inline Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Vector3 s = { std::sin(rotate.x), std::sin(rotate.y), std::sin(rotate.z) };
	Vector3 c = { std::cos(rotate.x), std::cos(rotate.y), std::cos(rotate.z) };
	return {
			scale.x * (c.y * c.z),
			scale.x * (c.y * s.z),
			scale.x * (-s.y),
			0.0f,

			scale.y * (s.x * s.y * c.z - c.x * s.z),
			scale.y * (s.x * s.y * s.z + c.x * c.z),
			scale.y * (s.x * c.y),
			0.0f,

			scale.z * (c.x* s.y* c.z + s.x * s.z),	
			scale.z * (c.x* s.y* s.z - s.x * c.z),	
			scale.z * (c.x* c.y),	
			0.0f,

			translate.x,	
			translate.y,	
			translate.z,	
			1.0f
	};
}

inline Vector3 GetXAxis(const Matrix4x4& m) {
	return { m.m[0][0],m.m[0][1],m.m[0][2] };
}

inline Vector3 GetYAxis(const Matrix4x4& m) {
	return { m.m[1][0],m.m[1][1],m.m[1][2] };
}

inline Vector3 GetZAxis(const Matrix4x4& m) {
	return { m.m[2][0],m.m[2][1],m.m[2][2] };
}

inline Vector3 GetTranslate(const Matrix4x4& m) {
	return { m.m[3][0],m.m[3][1],m.m[3][2] };
}


inline Vector3 operator*(const Vector3& v, const Matrix4x4& m) {
	return {
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2] };
}

