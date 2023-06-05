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

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, (int)kWindowWidth, (int)kWindowHeight);
	Input* input = Input::GetInstance();

	RenderingPipeline renderingPipeline{};
	renderingPipeline.Initalize(static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight));


	Vector3 rotate{ 0.0f,0.0f, 0.0f };
	rotate *= Math::ToRadian;
	Vector3 translate{ -30.0f,50.0f,20.0f };

	OBB obb{
		.center{-1.0f,0.0f,0.0f},
		.orientations = {Vector3UnitX,Vector3UnitY,Vector3UnitZ},
		.size{0.5f,0.5f,0.5f}
	};

	Segment segment{ .origin{Vector3Zero}, .diff{Vector3One} };

	uint32_t color = WHITE;
	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		///
		/// ↓更新処理ここから
		///

		MoveCamera(renderingPipeline);
		{
			ImGui::SetNextWindowPos({ (float)kWindowWidth - 330.0f, 0.0f }, ImGuiCond_Once);
			ImGui::SetNextWindowSize({ 330.0f, 500.0f }, ImGuiCond_Once);
			ImGui::Begin("Window");

			ImGui::DragFloat3("OBB center", &obb.center.x, 0.01f);
			Vector3 degrees = rotate * Math::ToDegree;
			ImGui::DragFloat3("OBB rotate", &degrees.x, 0.1f, -360.0f, 360.0f);
			rotate = degrees * Math::ToRadian;
			GetOrientations(MakeRotateXYZMatrix(rotate), obb.orientations);
			ImGui::DragFloat3("OBB size", &obb.size.x, 0.01f, 0.01f, 1000.0f);

			ImGui::DragFloat3("Segment origin", &segment.origin.x, 0.01f);
			ImGui::DragFloat3("Segment diff", &segment.diff.x, 0.01f);

			ImGui::End();
		}

		color = IsCollision(obb, segment) ? RED : WHITE;


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		renderingPipeline.DrawGrid(4);

		renderingPipeline.DrawSegment(segment, WHITE);
		renderingPipeline.DrawOBB(obb, color);

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
		constexpr float rotSpeed = 1.0f * Math::ToRadian;
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
	ImGui::DragFloat3("Position", &renderingPipeline.cameraPosition.x, 0.01f);
	Vector3 degrees = renderingPipeline.cameraRotate * Math::ToDegree;
	ImGui::DragFloat3("Rotate", &degrees.x, 0.1f, -360.0f, 360.0f);
	renderingPipeline.cameraRotate = degrees * Math::ToRadian;
	if (ImGui::Button("Reset")) {
		renderingPipeline.cameraRotate = { 15.0f * Math::ToRadian, 0.0f, 0.0f };
		Vector3 forward = GetZAxis(MakeRotateXYZMatrix(renderingPipeline.cameraRotate));
		renderingPipeline.cameraPosition = -forward * 6.0f;
	}
	ImGui::End();
}
