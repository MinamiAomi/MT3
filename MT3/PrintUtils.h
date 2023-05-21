#pragma once
#include "MyMath.h"

void VectorScreenPrintf(int x, int y, const Vector2& v, const char* label);
void VectorScreenPrintf(int x, int y, const Vector3& v, const char* label);
void MatrixScreenPrintf(int x, int y, const Matrix4x4& m, const char* label);

namespace ImGui {
	void TextVector(const char* label, const Vector3& v);
	void TextMatrix(const char* label, const Matrix4x4& m);
}