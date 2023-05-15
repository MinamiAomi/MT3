#include <Novice.h>
#include <Input.h>
#include <cstdint>
#include <ImGuiManager.h>
#include "MyMath.h"
#include "ReneringPipeline.h"
#include "Collision.h"

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

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, (int)kWindowWidth, (int)kWindowHeight);
	Input* input = Input::GetInstance();

	RenderingPipeline renderingPipeline{};
	renderingPipeline.Initalize(static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight));

	Segment segment{ { 0.0f, 1.5f, 1.0f }, {0.0f,-2.0f,-2.0f} };
	Triangle triangle{ {{0.0f,1.0f,0.0f},{1.0f,0.0f,0.0f},{-1.0f,0.0f,0.0f}} };

	uint32_t color = WHITE;
	Vector3 point{};

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
		ImGui::DragFloat3("Segment origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("Segment diff", &segment.diff.x, 0.01f);
		ImGui::DragFloat3("Triangle v0", &triangle.vertices[0].x, 0.01f);
		ImGui::DragFloat3("Triangle v1", &triangle.vertices[1].x, 0.01f);
		ImGui::DragFloat3("Triangle v2", &triangle.vertices[2].x, 0.01f);
		Intersection(triangle, segment, point);
		ImGui::Text("Intersection point : %.3f, %.3f, %.3f", point.x, point.y, point.z);
		ImGui::End();

		color = IsCollision(triangle, segment) ? RED : WHITE;

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		renderingPipeline.DrawGrid(4);

		renderingPipeline.DrawSegment(segment, color);
		renderingPipeline.DrawTriangle(triangle, color);
		renderingPipeline.DrawSphere({ point,0.02f }, GREEN);

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
