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

    Vector3 translates[3] = {
        {0.2f, 1.0f,0.0f},
        {0.4f, 0.0f,0.0f},
        {0.3f, 0.0f,0.0f},
    };
    Vector3 rotates[3] = {
        {0.0f, 0.0f,-6.8f},
        {0.0f, 0.0f,-1.4f},
        {0.0f, 0.0f,0.0f},
    };
    Vector3 scales[3] = {
       {1.0f, 1.0f,1.0f},
       {1.0f, 1.0f,1.0f},
       {1.0f, 1.0f,1.0f},
    };


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

            ImGui::DragFloat3("translates[0]", &translates[0].x, 0.01f);
            ImGui::DragFloat3Degree("rotates[0]", &rotates[0].x, 0.1f);
            ImGui::DragFloat3("scales[0]", &scales[0].x, 0.01f);

            ImGui::DragFloat3("translates[1]", &translates[1].x, 0.01f);
            ImGui::DragFloat3Degree("rotates[1]", &rotates[1].x, 0.1f);
            ImGui::DragFloat3("scales[1]", &scales[1].x, 0.01f);

            ImGui::DragFloat3("translates[2]", &translates[2].x, 0.01f);
            ImGui::DragFloat3Degree("rotates[2]", &rotates[2].x, 0.1f);
            ImGui::DragFloat3("scales[2]", &scales[2].x, 0.01f);

            ImGui::End();
        }

        Matrix4x4 shoulderLocalMatrix = MakeAffineMatrix(scales[0], rotates[0], translates[0]);
        Matrix4x4 elbowLocalMatrix = MakeAffineMatrix(scales[1], rotates[1], translates[1]);
        Matrix4x4 handLocalMatrix = MakeAffineMatrix(scales[2], rotates[2], translates[2]);

        Matrix4x4 shoulderWorldMatrix = shoulderLocalMatrix;
        Matrix4x4 elbowWorldMatrix = elbowLocalMatrix * shoulderWorldMatrix;
        Matrix4x4 handWorldMatrix = handLocalMatrix * elbowWorldMatrix;

        Vector3 shoulderPosition = GetTranslate(shoulderWorldMatrix);
        Vector3 elbowPosition = GetTranslate(elbowWorldMatrix);
        Vector3 handPosition = GetTranslate(handWorldMatrix);

        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        renderingPipeline.DrawGrid(4);

        renderingPipeline.DrawSphere({ GetTranslate(shoulderWorldMatrix), 0.1f }, RED);
        renderingPipeline.DrawSphere({ GetTranslate(elbowWorldMatrix), 0.1f }, GREEN);
        renderingPipeline.DrawSphere({ GetTranslate(handWorldMatrix), 0.1f }, BLUE);

        renderingPipeline.DrawLine(shoulderPosition, elbowPosition, WHITE);
        renderingPipeline.DrawLine(elbowPosition, handPosition, WHITE);

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
