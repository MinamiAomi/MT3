#include <Novice.h>
#include <Input.h>
#include <cstdint>
#include "MyMath.h"
#include "ReneringPipeline.h"
#include <ImGuiManager.h>

const char kWindowTitle[] = "LE2A_19_ミナミアオミ";
const uint32_t kWindowWidth = 1280;
const uint32_t kWindowHeight = 720;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, (int)kWindowWidth, (int)kWindowHeight);
	Input* input = Input::GetInstance();

	RenderingPipeline renderingPipeline{};

	// ビュー行列をセット
	constexpr float rad = Math::ToRad(20.0f);
	Vector3 cameraDir = Transform(kVector3UnitZ, MakeRotateXMatrix(rad));
	renderingPipeline.cameraRotate = { rad, 0.0f, 0.0f };
	renderingPipeline.cameraPosition = -cameraDir * 6.0f;
	// 射影行列をセット
	renderingPipeline.projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
	// ビューポート行列をセット
	renderingPipeline.viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
	// 行列を合成
	renderingPipeline.UpdateMatrix();
	renderingPipeline.SetCullMode(CullMode::kBack);
	renderingPipeline.SetIsWireFrame(true);



	Segment segment{ { -2.0f, -1.0f, 0.0f }, { 3.0f, 2.0f, 2.0f } };
	Vector3 point{ -1.5f, 0.6f, 0.6f };

	Vector3 project = Project(point - segment.origin, segment.diff);
	Vector3 closestPoint = ClosestPoint(point, segment);


	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		///
		/// ↓更新処理ここから
		///

		// 左入力でY軸に反時計回り、右入力でY軸に時計回り
		int32_t cameraMovement = input->PushKey(DIK_LEFT) - input->PushKey(DIK_RIGHT);
		if (cameraMovement != 0) {

			constexpr float rotSpeed = Math::ToRad(1.0f);
			renderingPipeline.cameraRotate.y += rotSpeed * cameraMovement;
			renderingPipeline.cameraPosition = renderingPipeline.cameraPosition * MakeRotateYMatrix(rotSpeed * cameraMovement);

			renderingPipeline.UpdateMatrix();
		}


		ImGui::SetNextWindowPos({ (float)kWindowWidth - 300.0f, 0.0f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 300.0f, 300.0f }, ImGuiCond_Once);
		ImGui::Begin("Window");
		ImGui::InputFloat3("Point", &point.x);
		ImGui::InputFloat3("Segment origin", &segment.origin.x);
		ImGui::InputFloat3("Segment diff", &segment.diff.x);
		ImGui::InputFloat3("Project", &project.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("ClosestPoint", &closestPoint.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::End();

		project = Project(point - segment.origin, segment.diff);
		closestPoint = ClosestPoint(point, segment);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		renderingPipeline.DrawGrid(4);

		renderingPipeline.DrawSphere({ point, 0.01f }, RED);
		renderingPipeline.DrawSphere({ closestPoint, 0.01f }, BLACK);

		renderingPipeline.DrawSegment(segment, WHITE);

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
