#include <Novice.h>
#include <Input.h>
#include <cstdint>
#include <ImGuiManager.h>
#include "MyMath.h"
#include "ReneringPipeline.h"
#include "Collision.h"
#include "PrintUtils.h"
#include "Physics.h"

#include "GJKEPA.h"

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

class SphereCollider : public Collider {
public:
    Vector3 FindFurthestPoint(const Vector3& direction) const {
        return sphere.center + Normalize(direction) * sphere.radius;
    }

    Geometry::Sphere sphere;
};

Vector3 Support(const Collider& collider1, const Collider& collider2, const Vector3& direction) {
    return collider1.FindFurthestPoint(direction) - collider2.FindFurthestPoint(-direction);
}

bool SameDirection(const Vector3& a, const Vector3& b) {
    return Dot(a, b) > 0;
}

bool LineCase(std::vector<Vector3>& simplex, Vector3& direction) {
    assert(simplex.size() == 2);
    Vector3 a = simplex[0];
    Vector3 b = simplex[1];

    Vector3 ab = b - a;
    Vector3 ao = -a;
    if (SameDirection(ab, ao)) {
        direction = Cross(Cross(ab, ao), ab);
    }
    else {
        simplex = { a };
        direction = ao;
    }
    return false;
}

bool TriangleCase(std::vector<Vector3>& simplex, Vector3& direction) {
    assert(simplex.size() == 3);
    Vector3 a = simplex[0];
    Vector3 b = simplex[1];
    Vector3 c = simplex[2];

    Vector3 ab = b - a;
    Vector3 ac = c - a;
    Vector3 ao = -a;

    Vector3 abc = Cross(ab, ac);

    if (SameDirection(Cross(abc, ac), ao)) {
        if (SameDirection(ac, ao)) {
            simplex = { a,c };
            direction = Cross(Cross(ac, ao), ac);
        }
        else {
            return LineCase(simplex = { a,b }, direction);
        }
    }
    else {
        if (SameDirection(Cross(ab, abc), ao)) {
            return LineCase(simplex = { a,b }, direction);
        }
        else {
            if (SameDirection(abc, ao)) {
                direction = abc;
            }
            else {
                simplex = { a,c,b };
                direction - abc;
            }
        }
    }
    return false;
}

bool Tetrahedron(std::vector<Vector3>& simplex, Vector3& direction) {
    assert(simplex.size() == 4);
    Vector3 a = simplex[0];
    Vector3 b = simplex[1];
    Vector3 c = simplex[2];
    Vector3 d = simplex[3];

    Vector3 ab = b - a;
    Vector3 ac = c - a;
    Vector3 ad = d - a;
    Vector3 ao = -a;

    Vector3 abc = Cross(ab, ac);
    Vector3 acd = Cross(ac, ad);
    Vector3 adb = Cross(ad, ab);

    if (SameDirection(abc, ao)) {
        return TriangleCase(simplex = { a,b,c }, direction);
    }
    if (SameDirection(acd, ao)) {
        return TriangleCase(simplex = { a,c,d }, direction);
    }
    if (SameDirection(adb, ao)) {
        return TriangleCase(simplex = { a,d,b }, direction);
    }
    return true;
}

bool NextSimplex(std::vector<Vector3>& simplex, Vector3& direction) {
    switch (simplex.size()) {
    case 2: return LineCase(simplex, direction);
    case 3: return TriangleCase(simplex, direction);
    case 4: return Tetrahedron(simplex, direction);
    }
    return false;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, (int)kWindowWidth, (int)kWindowHeight);
    Input* input = Input::GetInstance();


    RenderingPipeline renderingPipeline{};
    renderingPipeline.Initalize(static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight));


    SphereCollider sh1;
    SphereCollider sh2;

    sh1.sphere.center = { 1.0f,0.0f,0.0f };
    sh2.sphere.center = { -1.0f,0.0f,0.0f };
    sh1.sphere.radius = 0.2f;
    sh2.sphere.radius = 0.2f;

    uint32_t color = WHITE;

    bool canEdit = false;

    std::vector<Vector3> simplex;
    Vector3 direction = Vector3UnitX;
    Vector3 support = {};
    bool hit = false;
    bool gjkEnd = false;



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

            if (ImGui::Checkbox("Edit", &canEdit)) {
                hit = false;
                gjkEnd = false;
                if (!canEdit) {
                    simplex.clear();
                    direction = Vector3UnitX;
                    support = Support(sh1, sh2, direction);
                    simplex.emplace_back(support);
                    direction = -support;
                }

            }
            if (canEdit) {

                ImGui::DragFloat3("Sh1 center", &sh1.sphere.center.x, 0.01f);
                ImGui::DragFloat("Sh1 radius", &sh1.sphere.radius, 0.01f);

                ImGui::DragFloat3("Sh2 center", &sh2.sphere.center.x, 0.01f);
                ImGui::DragFloat("Sh2 radius", &sh2.sphere.radius, 0.01f);
            }
            else {
                if (ImGui::Button("Step")) {
                    assert(simplex.size() < 5);

                    support = Support(sh1, sh2, direction);

                    if (Dot(support, direction) <= 0) {
                        hit = false;
                        gjkEnd = true;
                    }

                    simplex.emplace_back(support);

                    if (NextSimplex(simplex, direction)) {
                        hit = true;
                        gjkEnd = true;
                    }
                }
                ImGui::TextVector3("Direction", direction);
            }


            ImGui::End();
        }

       // color = GJK(sh1, sh2, nullptr) ? RED : WHITE;

      

        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        renderingPipeline.DrawGrid(4);

        color = WHITE;
        if (gjkEnd) {
            color = hit ? RED : BLUE;
        }
        renderingPipeline.DrawSphere(sh1.sphere, color);
        renderingPipeline.DrawSphere(sh2.sphere, color);

        if (!canEdit) {
            renderingPipeline.DrawLine(Vector3Zero, direction, BLACK);
            const uint32_t colorTable[4] = {
                RED,
                GREEN,
                BLUE,
                0xFFFF00FF
            };
            if (simplex.size() > 1) {
                for (size_t i = 0; i < simplex.size(); ++i) {
                    renderingPipeline.DrawLine(simplex[i], simplex[(i + 1) % simplex.size()], BLACK);
                }
            }
            for (auto& p : simplex) {
                renderingPipeline.DrawSphere({ p,0.02f }, 0x00FFFFFF,6);
            }
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

