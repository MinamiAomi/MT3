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
Vector4 MakeQuaternionFromAngleAxis(float angle, const Vector3& axis);
Vector4 MakeQuaternionFromLookRotate(const Vector3& direction, const Vector3& up);
Matrix4x4 MakeLookAtMatrix(const Vector3& direction, const Vector3& up);
Vector4 MakeQuaternionFromEuler(const Vector3& euler);
Vector4 MakeQuaternionFromMatrix(const Matrix4x4& m);
Vector4 MakeQuaternionFromOrthonormal(const Vector3& x, const Vector3& y, const Vector3& z);
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
	float theta = std::acos(Dot(start, end));
	if (theta < 0) {
		theta *= -1;
	}
	float sin = std::sin(theta);
	float sinT = std::sin(t * theta);
	float sinInvT = std::sin((1.0f - t) * theta);
	return (1.0f / sin) * (sinInvT * start + sinT * end);
}
inline float GetAngle(const Vector4& q) {
	return std::acos(q.w) * 2.0f;
}
inline Vector3 GetAxis(const Vector4& q) {
	return Vector3{ q.x,q.y,q.z } *(1.0f / std::sin(std::acos(q.w)));
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, (int)kWindowWidth, (int)kWindowHeight);
	Input* input = Input::GetInstance();

	RenderingPipeline renderingPipeline{};
	renderingPipeline.Initalize(static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight));



	Vector3 target{};
	Vector3 position{ 0.0f,0.0f, -1.0f };
	Vector3 position2{ 0.0f,0.0f, -1.0f };
	Vector3 up{ 0,1,0 };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		///
		/// ↓更新処理ここから
		///

		MoveCamera(renderingPipeline);

		ImGui::SetNextWindowPos({ (float)kWindowWidth - 330.0f, 0.0f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 330.0f, 500.0f }, ImGuiCond_Once);
		ImGui::Begin("Window");

		ImGui::DragFloat3("Target", &target.x, 0.01f);
		ImGui::DragFloat3("Position", &position.x, 0.01f);
		ImGui::DragFloat3("Position2", &position2.x, 0.01f);
		ImGui::InputFloat3("Up", &up.x);


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
		renderingPipeline.DrawLine(position, position + up, 0x666666FF);
		//	renderingPipeline.DrawLine(position, target, 0x666666FF);
		{
			Vector3 axis{};
			float angle{};
			Vector4 rot = MakeQuaternionFromLookRotate(target - position, up);
			axis = GetAxis(rot);
			angle = GetAngle(rot);
			Matrix4x4 mat = MakeRotateMatrixFromQuaternion(rot);
			Matrix4x4 world = MakeScaleMatrix({ 0.3f,0.1f,0.5f }) * mat * MakeTranslateMatrix(position);
			renderingPipeline.DrawBox(world, RED);

			ImGui::Begin("Window");

			ImGui::Text("Angle\n %0.3f", angle);
			ImGui::TextVector3("Axis", axis);
			ImGui::TextVector4("Quaternion", rot);
			ImGui::TextMatrix("Matrix1", mat);
			ImGui::End();
		}

		{
			Vector3 axis{};
			float angle{};
			Matrix4x4 mat = MakeLookAtMatrix(target - position2, up);
			Vector4 rot = MakeQuaternionFromMatrix(mat);
			Matrix4x4 world2 = MakeScaleMatrix({ 0.3f,0.1f,0.5f }) * mat * MakeTranslateMatrix(position2);
			renderingPipeline.DrawBox(world2, 0x6666FF66);

			axis = GetAxis(rot);
			angle = GetAngle(rot);
			renderingPipeline.DrawLine({ position2, axis }, WHITE);

			ImGui::Begin("Window");
			ImGui::Text("-------------------\n");
			ImGui::TextMatrix("Matrix2", mat);
			ImGui::TextVector4("Quaternion2", rot);
			ImGui::Text("Angle\n %0.3f", angle);
			ImGui::TextVector3("Axis", axis);
			ImGui::End();
		}

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

Vector4 MakeQuaternionFromAngleAxis(float angle, const Vector3& axis) {
	float sin2 = std::sin(angle * 0.5f);
	Vector4 result{};
	result.x = axis.x * sin2;
	result.y = axis.y * sin2;
	result.z = axis.z * sin2;
	result.w = std::cos(angle * 0.5f);
	return result;
}

Vector4 MakeQuaternionFromLookRotate(const Vector3& direction, const Vector3& up) {
	Vector3 z = Normalize(direction);
	Vector3 x = Normalize(Cross(up, z));
	Vector3 y = Cross(z, x);
	return MakeQuaternionFromOrthonormal(x, y, z);
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
	Vector3 x{ m.m[0][0],m.m[0][1],m.m[0][2] };
	Vector3 y{ m.m[1][0],m.m[1][1],m.m[1][2] };
	Vector3 z{ m.m[2][0],m.m[2][1],m.m[2][2] };
	return MakeQuaternionFromOrthonormal(x, y, z);
}

Matrix4x4 MakeLookAtMatrix(const Vector3& direction, const Vector3& up) {
	Vector3 z = Normalize(direction);
	Vector3 x = Normalize(Cross(up, z));
	Vector3 y = Cross(z, x);
	return {
		x.x, x.y, x.z, 0.0f,
		y.x, y.y, y.z, 0.0f,
		z.x, z.y, z.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
}

Vector4 MakeQuaternionFromOrthonormal(const Vector3& x, const Vector3& y, const Vector3& z) {
	float trace = x.x + y.y + z.z;

	if (trace > 0.0f) {
		float s = std::sqrt(trace + 1.0f) * 0.5f;
		Vector4 result{};
		result.w = s;
		s = 0.25f / s;
		result.x = (y.z - z.y) * s;
		result.y = (z.x - x.z) * s;
		result.z = (x.y - y.x) * s;
		return result;
	}
	else if (x.x > y.y && x.x > z.z) {
		float s = std::sqrt(1.0f + x.x - y.y - z.z) * 0.5f;
		Vector4 result{};
		result.x = s;
		s = 0.25f / s;
		result.y = (x.y + y.x) * s;
		result.z = (z.x + x.z) * s;
		result.w = (y.z - z.y) * s;
		return result;
	}
	else if (y.y > z.z) {
		float s = std::sqrt(1.0f - x.x + y.y - z.z) * 0.5f;
		Vector4 result{};
		result.y = s;
		s = 0.25f / s;
		result.x = (x.y + y.x) * s;
		result.z = (y.z + z.y) * s;
		result.w = (z.x - x.z) * s;
		return result;
	}
	Vector4 result{};
	float s = std::sqrt(1.0f - x.x - y.y + z.z) * 0.5f;
	result.z = s;
	s = 0.25f / s;
	result.x = (z.x + x.z) * s;
	result.y = (y.z + z.y) * s;
	result.w = (x.y - y.x) * s;
	return result;
}