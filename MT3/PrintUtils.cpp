#include "PrintUtils.h"
#include "Novice.h"
#include "ImGuiManager.h"

void VectorScreenPrintf(int x, int y, const Vector2& v, const char* label) {
	constexpr int kColumnWidth = 60;
	Novice::ScreenPrintf(x, y, "%.02f", v.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", v.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%s", label);
}

void VectorScreenPrintf(int x, int y, const Vector3& v, const char* label) {
	constexpr int kCoulumnWidth = 60;
	Novice::ScreenPrintf(x, y, "%.02f", v.x);
	Novice::ScreenPrintf(x + kCoulumnWidth, y, "%.02f", v.y);
	Novice::ScreenPrintf(x + kCoulumnWidth * 2, y, "%.02f", v.z);
	Novice::ScreenPrintf(x + kCoulumnWidth * 3, y, "%s", label);
}

void MatrixScreenPrintf(int x, int y, const Matrix4x4& m, const char* label) {
	constexpr int kRowHeight = 20;
	constexpr int kColumnWidth = 60;
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth,
				y + (row + 1) * kRowHeight, "%6.02f", m.m[row][column]);
		}
	}
}

namespace ImGui {
	void TextVector(const char* label, const Vector3& v) {
		std::string str(label);
		str += "\n";
		str += "%0.3f, %0.3f, %0.3f\n";
		ImGui::Text(str.c_str(),
			v.x, v.y, v.z);
	}

	void TextMatrix(const char* label, const Matrix4x4& m) {
		std::string str(label);
		str += "\n";
		str += "%0.3f, %0.3f, %0.3f, %0.3f\n";
		str += "%0.3f, %0.3f, %0.3f, %0.3f\n";
		str += "%0.3f, %0.3f, %0.3f, %0.3f\n";
		str += "%0.3f, %0.3f, %0.3f, %0.3f\n";

		ImGui::Text(str.c_str(),
			m.m[0][0], m.m[0][1], m.m[0][2], m.m[0][3],
			m.m[1][0], m.m[1][1], m.m[1][2], m.m[1][3],
			m.m[2][0], m.m[2][1], m.m[2][2], m.m[2][3],
			m.m[3][0], m.m[3][1], m.m[3][2], m.m[3][3]);
	}
}

