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

#include <string>
#include <fstream>

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

    Vector3 controlPoint0 = { -0.8f,0.58f,1.0f };
    Vector3 controlPoint1 = { 1.76f,1.0f,-0.3f };
    Vector3 controlPoint2 = { 0.94f,-0.7f,2.3f };
    Vector3 controlPoint3 = { -0.53f,-0.26f,-0.15f };


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

            ImGui::DragFloat3("ControlPoint[0]", &controlPoint0.x, 0.01f);
            ImGui::DragFloat3("ControlPoint[1]", &controlPoint1.x, 0.01f);
            ImGui::DragFloat3("ControlPoint[2]", &controlPoint2.x, 0.01f);
            ImGui::DragFloat3("ControlPoint[3]", &controlPoint3.x, 0.01f);

            ImGui::End();
        }


        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        renderingPipeline.DrawGrid(4);

        renderingPipeline.DrawCatmullRom(controlPoint0, controlPoint1, controlPoint2, controlPoint3);

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
    ImGui::DragFloat3("Camera position", &renderingPipeline.cameraPosition.x, 0.01f);
    Vector3 deg = renderingPipeline.cameraRotate * Math::ToDegree;
    ImGui::DragFloat3("Camera rotate", &deg.x, 0.1f, 0.0f, 360.0f);
    renderingPipeline.cameraRotate = deg * Math::ToRadian;
    if (ImGui::Button("Camera reset")) {
        renderingPipeline.cameraRotate = { 15.0f * Math::ToDegree, 0.0f, 0.0f };
        Vector3 forward = GetZAxis(MakeRotateXYZMatrix(renderingPipeline.cameraRotate));
        renderingPipeline.cameraPosition = -forward * 6.0f;
    }
    ImGui::End();
}
