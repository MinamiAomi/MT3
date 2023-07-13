#include <Novice.h>
#include <Input.h>
#include <cstdint>
#include <ImGuiManager.h>
#include "MyMath.h"
#include "ReneringPipeline.h"
#include "Collision.h"
#include "PrintUtils.h"
#include "Physics.h"

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

Geometry::Ray MouseRay(const RenderingPipeline& renderingPipeline);

struct Ball : public PhysicsObject {
    float radius;
    uint32_t color;
};

Vector3 RandomPoint(const Vector3& min, const Vector3& max) {
    Vector3 p{};
    constexpr float r = 1.0f / RAND_MAX;
    p.x = Math::Lerp(min.x, max.x, std::rand() * r);
    p.y = Math::Lerp(min.y, max.y, std::rand() * r);
    p.z = Math::Lerp(min.z, max.z, std::rand() * r);
    return p;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, (int)kWindowWidth, (int)kWindowHeight);
    Input* input = Input::GetInstance();


    RenderingPipeline renderingPipeline{};
    renderingPipeline.Initalize(static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight));

    const size_t kObjCount = 6;
    const size_t kSpringCount = (kObjCount - 1);

    Ball balls[kObjCount]{};
    for (size_t i = 0; i < kObjCount; ++i) {
        balls[i].position = RandomPoint({ -1,-1,-1 }, { 1,1,1 });
    }
    Spring springs[kObjCount][kSpringCount]{};

    for (size_t i = 0; i < kObjCount; ++i) {
        balls[i].mass = 2.0f;
        balls[i].radius = 0.05f;
        balls[i].color = BLUE;
        size_t p = 0;
        for (size_t j = 0; j < kSpringCount; ++j, ++p) {
            if (i == j) { ++p; }
            springs[i][j].anchor = balls[p].position;
            springs[i][j].naturalLength = 0.7f;
            springs[i][j].stiffness = 100.0f;
            springs[i][j].damping = 2.0f;
        }
    }


    float deltaTime = 1.0f / 60.0f;

    Ball* dragBall = nullptr;
    bool stop = true;

    const Vector3 gravityAcceleration = -Vector3UnitY * 9.8f;

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

            stop = stop ? !ImGui::Button("Start", { 50,0 }) : ImGui::Button("Stop", { 50,0 });

          /*  for (size_t i = 0; i < 4; ++i) {
                balls[i].ShowUI(("Ball" + std::to_string(i)).c_str());
                for (size_t j = 0; j < 3; ++j) {
                    springs[i][j].ShowUI(("Spring" + std::to_string(i) + std::to_string(j)).c_str());
                }
            }*/


            ImGui::End();
        }

        if (!stop) {
           
            for (size_t i = 0; i < kObjCount; ++i) {
                size_t p = 0;
                for (size_t j = 0; j < kSpringCount; ++j, ++p) {
                    if (i == j) { ++p; }
                    springs[i][j].anchor = balls[p].position;
                    balls[i].AddForce(springs[i][j].ComputeForce(balls[i]));
                }
                balls[i].AddForce(gravityAcceleration * balls[i].mass);
            }
            for (size_t i = 0; i < kObjCount; ++i) {
                balls[i].UpdatePosition(deltaTime);
            }
        }

        // ドラッグ処理
        if (input->IsPressMouse(0)) {
            if (!dragBall) {
                auto ray = MouseRay(renderingPipeline);
                for (size_t i = 0; i < kObjCount; ++i) {
                    if (Collision::IsCollision(Geometry::Sphere{ balls[i].position, balls[i].radius }, ray)) {
                        dragBall = &balls[i];
                    }
                }
            }
        }
        else { dragBall = nullptr; }

        if (dragBall) {
            auto ray = MouseRay(renderingPipeline);
            float distance = Length(dragBall->position - ray.origin);
            dragBall->position = ray.origin + Normalize(ray.diff) * distance;
            dragBall->velocity = Vector3Zero;
        }

        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        renderingPipeline.DrawGrid(4);



        for (size_t i = 0; i < kObjCount; ++i) {
            for (size_t j = 0; j < kSpringCount; ++j) {
                renderingPipeline.DrawLine(balls[i].position, springs[i][j].anchor, WHITE);
            }
        }
        for (size_t i = 0; i < kObjCount; ++i) {
            renderingPipeline.DrawSphere({ balls[i].position,balls[i].radius }, dragBall != &balls[i] ? balls[i].color : RED);
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

Geometry::Ray MouseRay(const RenderingPipeline& renderingPipeline) {
    auto input = Input::GetInstance();

    Vector2 screenMousePosition = input->GetMousePosition();

    Vector3 mouseNear = { screenMousePosition.x, screenMousePosition.y, 0.0f };
    Vector3 mouseFar = { screenMousePosition.x, screenMousePosition.y, 1.0f };

    mouseNear = Transform(mouseNear, renderingPipeline.GetVPVMatrixInverse());
    mouseFar = Transform(mouseFar, renderingPipeline.GetVPVMatrixInverse());

    Geometry::Ray ray{};
    ray.origin = mouseNear;
    ray.diff = mouseFar - mouseNear;
    return ray;
}

