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

void ImGuiRotateDeg(const std::string& label, Vector3& rotate);

Matrix4x4 MakeViewMatrix(const Vector3& pos, const Vector3& rot);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, (int)kWindowWidth, (int)kWindowHeight);
	Input* input = Input::GetInstance();

	RenderingPipeline renderingPipeline{};
	renderingPipeline.Initalize(static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight));

	uint32_t width = 64;
	uint32_t height = 36;

	Vector3 cameraPos{ 0.0f,0.0f,0.0f };
	Vector3 cameraRot{};

	float fovY = 45.0f;
	float aspectRaito = static_cast<float>(width) / static_cast<float>(height);
	float nearZ = 1.0f;
	float farZ = 125.0f;

	Matrix4x4 viewMat = MakeViewMatrix(cameraPos, cameraRot);
	Matrix4x4 projMat = MakePerspectiveFovMatrix(Math::ToRad(fovY), aspectRaito, nearZ, farZ);
	Matrix4x4 viewProjMat = viewMat * projMat;
	Matrix4x4 viewProjMatInv = Inverse(viewProjMat);

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		///
		/// ↓更新処理ここから
		///

		MoveCamera(renderingPipeline);

		ImGui::SetNextWindowPos({ (float)kWindowWidth - 330.0f, 0.0f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 330.0f, 300.0f }, ImGuiCond_Once);
		ImGui::Begin("Ray");
		ImGui::DragFloat3("Camera pos", &cameraPos.x, 0.01f);
		ImGuiRotateDeg("Camera rot", cameraRot);
		ImGui::SliderAngle("Fov Y", &fovY, 0.0f, 179.0f);
		int w = static_cast<int>(width);
		ImGui::DragInt("Width", &w, 1, 0);
		width = static_cast<uint32_t>(w);
		int h = static_cast<int>(height);
		ImGui::DragInt("Height", &h, 1, 0);
		height = static_cast<uint32_t>(h);
		aspectRaito = static_cast<float>(width) / static_cast<float>(height);
		ImGui::DragFloat("NearZ", &nearZ, 0.1f, 0.0f);
		ImGui::DragFloat("FarZ", &farZ, 0.1f, 0.0f);
		nearZ = (std::min)(nearZ, farZ);
		farZ = (std::max)(nearZ, farZ);
		ImGui::Text("Aspect raito : %.3f", aspectRaito);
		ImGui::Text("Ray count : %d", width * height);
		if (ImGui::Button("reset")) {
			width = 64;
			height = 36;
			cameraPos = { 0.0f,0.0f,0.0f };
			cameraRot = {};
			fovY = 45.0f;
			aspectRaito = static_cast<float>(width) / static_cast<float>(height);
			nearZ = 1.0f;
			farZ = 125.0f;

		}
		ImGui::End();

		viewMat = MakeViewMatrix(cameraPos, cameraRot);
		projMat = MakePerspectiveFovMatrix(fovY, aspectRaito, nearZ, farZ);
		viewProjMat = viewMat * projMat;
		viewProjMatInv = Inverse(viewProjMat);


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		renderingPipeline.DrawGrid(10);

		{
			Vector3 vertices[] = {
				{  1.0f,  1.0f, 0.0f },
				{ -1.0f,  1.0f, 0.0f },
				{ -1.0f, -1.0f, 0.0f },
				{  1.0f, -1.0f, 0.0f },
			};
			for (auto& vertex : vertices) {
				vertex = Transform(vertex, viewProjMatInv);
			}
			renderingPipeline.DrawLine(vertices[0], vertices[1], RED);
			renderingPipeline.DrawLine(vertices[1], vertices[2], RED);
			renderingPipeline.DrawLine(vertices[2], vertices[3], RED);
			renderingPipeline.DrawLine(vertices[3], vertices[0], RED);
		}

		{
			Vector3 rayOrigin = cameraPos;
			//Vector4 v = Multiply({ 0.0f,0.0f,0.0f,1.0f }, viewProjMatInv);
			Vector3 g{ viewProjMatInv.m[3][0],viewProjMatInv.m[3][1],viewProjMatInv.m[3][2] };
			rayOrigin = g;

			Vector3 lineStart = renderingPipeline.Apply(cameraPos);
			Vector2 dimention{ static_cast<float>(width), static_cast<float>(height) };
			for (uint32_t y = 0; y < height; y++) {
				for (uint32_t x = 0; x < width; x++) {
					Vector2 launchIndex{ static_cast<float>(x), static_cast<float>(y) };
					Vector2 xy = launchIndex + Vector2(0.5f, 0.5f);
					/*xy.x = xy.x / dimention.x * 2.0f - 1.0f;
					xy.y = 1.0f - xy.y / dimention.y * 2.0f;*/

					xy.x *= 2.0f / dimention.x;
					xy.y *= -2.0f / dimention.y;
					xy.x += -1.0f;
					xy.y += 1.0f;

					Vector4 hom = Multiply({ xy.x,xy.y,0.0f,1.0f }, viewProjMatInv);
					Vector3 worldPos{ hom.x,hom.y,hom.z };
					 //worldPos *= 1.0f / hom.w;	
					Vector3 rayDirection = worldPos - rayOrigin;

					Vector3 lineEnd = renderingPipeline.Apply(rayDirection + rayOrigin);
					renderingPipeline.ScreenDrawLine(lineStart, lineEnd, GREEN);
				}
			}
		}
		{
			Ray ray{};
			ray.origin = cameraPos;
			ray.diff = TransformNormal(kVector3UnitZ * 5, MakeRotateXYZMatrix(cameraRot));
			renderingPipeline.DrawRay(ray, BLACK);
		}
		renderingPipeline.DrawSphere({ cameraPos, 0.02f }, BLACK);

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
	ImGuiRotateDeg("Camera rotate", renderingPipeline.cameraRotate);
	if (ImGui::Button("Camera reset")) {
		renderingPipeline.cameraRotate = { Math::ToRad(15.0f), 0.0f, 0.0f };
		Vector3 forward = GetZAxis(MakeRotateXYZMatrix(renderingPipeline.cameraRotate));
		renderingPipeline.cameraPosition = -forward * 6.0f;
	}
	ImGui::End();
}

void ImGuiRotateDeg(const std::string& label, Vector3& rotate) {
	float deg[3] = { Math::ToDeg(rotate.x), Math::ToDeg(rotate.y), Math::ToDeg(rotate.z) };
	ImGui::DragFloat3(label.c_str(), deg, 1.0f, 0.0f, 360.0f);
	rotate = { Math::ToRad(deg[0]), Math::ToRad(deg[1]), Math::ToRad(deg[2]) };
}

Matrix4x4 MakeViewMatrix(const Vector3& translate, const Vector3& rotate) {
	Matrix4x4 cameraMat = MakeRotateXYZMatrix(rotate) * MakeTranslateMatrix(translate);
	return Inverse(cameraMat);
}
