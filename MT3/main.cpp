#include <Novice.h>
#include <Input.h>
#include <cstdint>
#include <ImGuiManager.h>
#include "MyMath.h"
#include "ReneringPipeline.h"
#include "Collision.h"
#include "PrintUtils.h"

const char kWindowTitle[] = "LE2A_19_ミナミアオミ";
const uint32_t kWindowWidth = 1280;
const uint32_t kWindowHeight = 720;

/// <summary>
/// 右クリックを押すと向き
/// ホイールクリックで左右移動
/// ホイールで前後移動
/// </summary>
/// <param name="renderingPipeline"></param>
void MoveCamera(RenderingPipeline& renderingPipeline);

Matrix4x4 MakeRotateMatrixFromQuaternion(const Vector4& q);
Vector4 MakeQuaternionFromEuler(const Vector3& euler);
Vector4 MakeQuaternionFromMatrix(const Matrix4x4& m);
float Dot(const Vector4& v1, const Vector4& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}
Vector4 operator+(const Vector4& v1, const Vector4& v2) {
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w };
}
Vector4 operator*(float s, const Vector4& v) {
	return { v.x * s, v.y * s, v.z * s, v.w * s };
}
Vector4 Slerp(float t, const Vector4& start, const Vector4& end) {
//	Vector4 s = start;
//	float dot_val = Dot(start, end);
//	// q1, q2が反対向きの場合
//	if (dot_val < 0) {
//		s.w = -s.w;
//		s.x = -s.x;
//		s.y = -s.y;
//		s.z = -s.z;
//		dot_val = -dot_val;
//	}
//	// 球面線形補間の計算
//	float theta = std::acos(dot_val);
//	return (std::sin((1 - t) * theta) * s + std::sin(t * theta) * end) * (1.0f / std::sin(theta));

	float dot = Dot(start, end);
	float theta = std::acos(dot);
	float sin = std::sin(theta);
	float sinT = std::sin(t * theta);
	float sinInvT = std::sin((1.0f - t) * theta);
	Vector4 a = (sinInvT / sin) * start;
	Vector4 b = (sinT / sin) * end;
	return a + b;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, (int)kWindowWidth, (int)kWindowHeight);
	Input* input = Input::GetInstance();

	RenderingPipeline renderingPipeline{};
	renderingPipeline.Initalize(static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight));



	Vector3 target{};
	Vector3 position{};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		///
		/// ↓更新処理ここから
		///

		MoveCamera(renderingPipeline);

		ImGui::SetNextWindowPos({ (float)kWindowWidth - 330.0f, 0.0f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 330.0f, 170.0f }, ImGuiCond_Once);
		ImGui::Begin("Window");

		ImGui::DragFloat3("Target", &target.x, 0.01f);
		ImGui::DragFloat3("Position", &position.x, 0.01f);


		ImGui::End();



		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		renderingPipeline.DrawGrid(4);

		renderingPipeline.DrawSphere({ target,0.05f }, WHITE);
		renderingPipeline.DrawSphere({ position,0.05f }, BLACK);
		renderingPipeline.DrawLine(position, target, 0x666666FF);

		renderingPipeline.DrawAxis();

		///
		/// ↑描画処理ここまで
		///


		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (input->TriggerKey(DIK_ESCAPE)) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

void MoveCamera(RenderingPipeline& renderingPipeline) {
	Input* input = Input::GetInstance();

	auto mouseMove = input->GetMouseMove();
	auto wheel = input->GetWheel();

	if (input->IsPressMouse(1)) {
		constexpr float rotSpeed = Math::ToRad(1.0f);
		renderingPipeline.cameraRotate.x += rotSpeed * static_cast<float>(mouseMove.lY) * 0.1f;
		renderingPipeline.cameraRotate.y += rotSpeed * static_cast<float>(mouseMove.lX) * 0.1f;
	}
	else if (input->IsPressMouse(2)) {
		Matrix4x4 rotMat = MakeRotateXYZMatrix(renderingPipeline.cameraRotate);
		Vector3 cameraX = GetXAxis(rotMat) * (-static_cast<float>(mouseMove.lX) * 0.01f);
		Vector3 cameraY = GetYAxis(rotMat) * (static_cast<float>(mouseMove.lY) * 0.01f);
		renderingPipeline.cameraPosition += cameraX + cameraY;
	}
	else if (wheel != 0) {
		Matrix4x4 rotMat = MakeRotateXYZMatrix(renderingPipeline.cameraRotate);
		Vector3 cameraZ = GetZAxis(rotMat) * (static_cast<float>(wheel / 120) * 0.5f);
		renderingPipeline.cameraPosition += cameraZ;
	}

	renderingPipeline.UpdateMatrix();

	ImGui::SetNextWindowPos({ 0.0f, 0.0f }, ImGuiCond_Once);
	ImGui::SetNextWindowSize({ 300.0f, 100.0f }, ImGuiCond_Once);
	ImGui::Begin("Camera");
	ImGui::DragFloat3("Camera position", &renderingPipeline.cameraPosition.x, 0.01f);
	ImGui::DragFloat3("Camera rotate", &renderingPipeline.cameraRotate.x, 0.01f, 0.0f, Math::TwoPi);
	if (ImGui::Button("Camera reset")) {
		renderingPipeline.cameraRotate = { Math::ToRad(15.0f), 0.0f, 0.0f };
		Vector3 forward = GetZAxis(MakeRotateXYZMatrix(renderingPipeline.cameraRotate));
		renderingPipeline.cameraPosition = -forward * 6.0f;
	}
	ImGui::End();
}


Matrix4x4 MakeRotateMatrixFromQuaternion(const Vector4& q) {
	float w2 = q.w * q.w, x2 = q.x * q.x, y2 = q.y * q.y, z2 = q.z * q.z;
	float wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;
	float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;

	return {
		w2 + x2 - y2 - z2,	2.0f * (wz + xy),	2.0f * (xz - wy),	0.0f,
		2.0f * (xy - wz),	w2 - x2 + y2 - z2,	2.0f * (yz + wx),	0.0f,
		2.0f * (wy + xz),	2.0f * (-wx + yz),	w2 - x2 - y2 + z2,	0.0f,
		0.0f,				0.0f,				0.0f,				1.0f };
}

Vector4 MakeQuaternionFromEuler(const Vector3& euler) {
	// ピッチ ヨー ロールの順
	Vector3 s = Vector3(std::sin(euler.x * 0.5f), std::sin(euler.y * 0.5f), std::sin(euler.z * 0.5f));
	Vector3 c = Vector3(std::cos(euler.x * 0.5f), std::cos(euler.y * 0.5f), std::cos(euler.z * 0.5f));
	Vector4 result{};
	result.x = s.x * c.y * c.z - c.x * s.y * s.z;
	result.y = c.x * s.y * c.z + s.x * c.y * s.z;
	result.z = c.x * c.y * s.z - s.x * s.y * c.z;
	result.w = c.x * c.y * c.z + s.x * s.y * s.z;
	return result;
}

Vector4 MakeQuaternionFromMatrix(const Matrix4x4& m) {
	float elem[4] = {
		 m.m[0][0] - m.m[1][1] - m.m[2][2] + 1.0f,
		-m.m[0][0] + m.m[1][1] - m.m[2][2] + 1.0f,
		-m.m[0][0] - m.m[1][1] + m.m[2][2] + 1.0f,
		 m.m[0][0] + m.m[1][1] + m.m[2][2] + 1.0f,
	};

	uint32_t biggestIndex = 0;
	for (uint32_t i = 1; i < 4; ++i) {
		if (elem[i] > elem[biggestIndex]) {
			biggestIndex = i;
		}
	}
	if (elem[biggestIndex] < 0.0f) {
		return {};
	}
	Vector4 result{};
	float v = std::sqrt(elem[biggestIndex]) * 0.5f;
	(&result.x)[biggestIndex] = v;
	float mult = 0.25f / v;

	switch (biggestIndex)
	{
	case 0: // x
		result.y = (m.m[0][1] + m.m[1][0]) * mult;
		result.z = (m.m[2][0] + m.m[0][2]) * mult;
		result.w = (m.m[1][2] - m.m[2][1]) * mult;
		break;
	case 1: // y
		result.x = (m.m[0][1] + m.m[1][0]) * mult;
		result.z = (m.m[1][2] + m.m[2][1]) * mult;
		result.w = (m.m[2][0] - m.m[0][2]) * mult;
		break;
	case 2: // z
		result.x = (m.m[2][0] + m.m[0][2]) * mult;
		result.y = (m.m[1][2] + m.m[2][1]) * mult;
		result.w = (m.m[0][1] - m.m[1][0]) * mult;
		break;
	case 3: // w
		result.x = (m.m[1][2] - m.m[2][1]) * mult;
		result.y = (m.m[2][0] - m.m[0][2]) * mult;
		result.z = (m.m[0][1] - m.m[1][0]) * mult;
		break;
	}
	return result;
}