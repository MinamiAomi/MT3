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

Geometry::Ray MouseRay(const RenderingPipeline& renderingPipeline);

class Object;

struct PhysicsObject {
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    float mass;

    void AddForce(const Vector3& force) {
        acceleration += force * (1.0f / mass);
    }

    void UpdatePosition(float deltaTime) {
        velocity += acceleration * deltaTime;
        position += velocity * deltaTime;
        acceleration = Vector3Zero;
    }
};

struct Spring {
    bool active;
    Vector3 anchor; // アンカー
    float naturalLength; // 自然長
    float stiffness; // 硬さ
    float dempingCoefficient; // 減衰係数

    bool Gui(const char* label) {
        if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Active", &active);
            ImGui::DragFloat3("Anchor", &anchor.x, 0.01f);
            ImGui::DragFloat("NaturalLength ", &naturalLength, 0.1f);
            ImGui::DragFloat("Stiffness", &stiffness, 0.1f);
            ImGui::DragFloat("DempingCoefficient", &dempingCoefficient, 0.1f);
            ImGui::TreePop();
            return true;
        }
        return false;
    }
};

struct Ball : public PhysicsObject {
    float radius;
    uint32_t color;
};

Vector3 GravityForce(const PhysicsObject& o) {
    const Vector3 gravityAcceleration = -Vector3UnitY * 9.8f;
    return gravityAcceleration * o.mass;
}

Vector3 SpringForce(const PhysicsObject& o, const Spring& s) {
    Vector3 difference = o.position - s.anchor;
    float length = Length(difference);
    if (length == 0.0f) {
        return {};
    }
    Vector3 direction = difference * (1.0f / length);
    Vector3 restPosition = s.anchor + direction * s.naturalLength;
    // 変位ベクトル
    Vector3 displacement = length * (o.position - restPosition);
    // 復元力
    Vector3 restoringForce = -s.stiffness * displacement;
    // 減衰力
    Vector3 dempingForce = -s.dempingCoefficient * o.velocity;
    return restoringForce + dempingForce;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, (int)kWindowWidth, (int)kWindowHeight);
    Input* input = Input::GetInstance();

    RenderingPipeline renderingPipeline{};
    renderingPipeline.Initalize(static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight));

    Ball ball{};
    ball.position = { 1.2f, 0.0f, 0.0f };
    ball.mass = 2.0f;
    ball.radius = 0.05f;
    ball.color = BLUE;


    std::vector<Spring> springs(1);
    springs[0].active = true;
    springs[0].anchor = { 0.0f, 0.0f, 0.0f };
    springs[0].naturalLength = 1.0f;
    springs[0].stiffness = 100.0f;
    springs[0].dempingCoefficient = 2.0f;

    float deltaTime = 1.0f / 60.0f;

    bool isDrag = false;

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
            ImGui::DragFloat3("Ball position", &ball.position.x, 0.01f);
            ImGui::DragFloat3("Ball velocity", &ball.velocity.x, 0.01f);
            if (ImGui::Button("Add Spring")) {
                Spring s{};
                s.active = true;
                s.anchor = { 0.0f, 0.0f, 0.0f };
                s.naturalLength = 1.0f;
                s.stiffness = 100.0f;
                s.dempingCoefficient = 2.0f;
                springs.emplace_back(s);
            }
            for (auto& s : springs) {
                size_t index = (&s) - (&springs[0]);
                s.Gui(("Spring" + std::to_string(index)).c_str());
            }
            ImGui::End();
        }

        // 重力を計算する
        //ball.AddForce(GravityForce(ball));

        // すべてのばねの力を計算する
        for (auto& s : springs) {
            if (s.active) {
                ball.AddForce(SpringForce(ball, s));
            }
        }

        ball.UpdatePosition(deltaTime);

        // ドラッグ処理
        if (input->IsPressMouse(0)) {
            if (!isDrag) {
                auto ray = MouseRay(renderingPipeline);
                if (Collision::IsCollision(Geometry::Sphere{ ball.position, ball.radius }, ray)) {
                    isDrag = true;
                }
            }
        }
        else { isDrag = false; }

        if (isDrag) {
            auto ray = MouseRay(renderingPipeline);
            float distance = Length(ball.position - ray.origin);
            ball.position = ray.origin + Normalize(ray.diff) * distance;
            ball.velocity = Vector3Zero;
        }

        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        renderingPipeline.DrawGrid(4);

        for (auto& s : springs) {
            if (s.active) {
                renderingPipeline.DrawLine(ball.position, s.anchor, WHITE);
                renderingPipeline.DrawSphere({ s.anchor, 0.02f }, BLACK);
            }
        }
        renderingPipeline.DrawSphere({ ball.position,ball.radius }, !isDrag ? ball.color : RED);

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

