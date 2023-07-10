#define NOMINMAX
#include <Novice.h>
#include <Input.h>
#include <ImGuiManager.h>

#include <cstdint>
#include <vector>
#include <stack>
#include <span>
#include <algorithm>

#include "MyMath.h"
#include "ReneringPipeline.h"
#include "Collision.h"
#include "PrintUtils.h"
#include "ConvexHull.h"
#include "GJK.h"

const char kWindowTitle[] = "LE2A_19_ミナミアオミ";
const uint32_t kWindowWidth = 1280;
const uint32_t kWindowHeight = 720;

class Scene {
public:
    Scene(float width, float height) : width_(width), height_(height) {}

    void Update() {
        Input* input = Input::GetInstance();
        if (input->IsPressMouse(1)) {
            auto mouseMove = input->GetMouseMove();
            cameraPosition_.x -= float(mouseMove.lX) * 2.0f;
            cameraPosition_.y += float(mouseMove.lY) * 2.0f;
            translateMatrix_ = MakeTranslateMatrix(-cameraPosition_) * screenOffsetMatrix_;
        }
    }

    void DrawArea() {
        float hw = width_ * 0.5f;
        float hh = height_ * 0.5f;
        Vector3 screenPos = Vector3{ -hw, -hh, 0.0f } *translateMatrix_;
        Novice::DrawBox(int(screenPos.x), int(screenPos.y), int(width_), -int(height_), 0.0f, 0x00440033, kFillModeSolid);
        DrawLine({ hw, 0.0f }, { -hw, 0.0f }, 0xFFFFFF33);
        DrawLine({ 0.0f, hh }, { 0.0f, -hh }, 0xFFFFFF33);
    }

    void DrawCircle(const Vector2& c, float r, unsigned int color) {
        Vector3 screenPos = Vector3{ c.x, c.y, 0.0f } *translateMatrix_;
        Novice::DrawEllipse(int(screenPos.x), int(screenPos.y), int(r), int(r), 0.0f, color, kFillModeWireFrame);
    }

    void DrawLine(const Vector2& s, const Vector2& e, unsigned int color) {
        Vector3 ss = Vector3{ s.x, s.y, 0.0f } *translateMatrix_;
        Vector3 se = Vector3{ e.x, e.y, 0.0f } *translateMatrix_;
        Novice::DrawLine(int(ss.x), int(ss.y), int(se.x), int(se.y), color);
    }

private:
    float width_;
    float height_;
    Vector3 cameraPosition_{ 0.0f, 0.0f, 0.0f };
    Matrix4x4 screenOffsetMatrix_ = MakeScaleMatrix({ 1.0f,-1.0f,1.0f }) * MakeTranslateMatrix({ kWindowWidth * 0.5f, kWindowHeight * 0.5f, 0.0f });
    Matrix4x4 translateMatrix_ = MakeTranslateMatrix(-cameraPosition_) * screenOffsetMatrix_;
};

Scene scene(500.0f, 500.0f);

std::vector<Vector2> RandomPoints(float width, float height, size_t count) {
    std::vector<Vector2> points(count);
    width *= 0.5f;
    height *= 0.5f;
    for (auto& v : points) {
        v.x = Math::Lerp(-width, width, std::rand() / float(RAND_MAX));
        v.y = Math::Lerp(-height, height, std::rand() / float(RAND_MAX));
    }
    return points;
}

struct Transformation {
    Vector3 translate;
    Vector3 rotate;
    Vector3 scale;
    Matrix4x4 world;
};

class MeshCollider :
    public CollisionShape {
public:
    void AddVertex(const Vector2& v) { vertices_.emplace_back(v); }
    Vector2 FindFurthestPoint(const Vector2& direction) const override {
        auto iter = vertices_.begin();
        auto findPoint = iter;
        float maxDistance = Dot(*iter++, direction);
        for (; iter != vertices_.end(); ++iter) {
            float d = Dot(*iter, direction);
            if (d > maxDistance) {
                maxDistance = d;
                findPoint = iter;
            }
        }
        return *findPoint;
    }
    Vector2 ArbitraryPoint() const override {
        Vector2 center{};
        for (auto& v : vertices_) {
            center += v;
        }
        return center * (1.0f / vertices_.size());
    }
    void Draw(unsigned int color) {
        for (auto& v : vertices_) {
            scene.DrawCircle(v, 2.0f, color);
        }
    }
private:
    std::vector<Vector2> vertices_;
};

class CircleCollider :
    public CollisionShape {
public:

    CircleCollider(const Vector2& c, float r) : center_(c), radius_(r) {}
    Vector2 FindFurthestPoint(const Vector2& direction) const override {
        return Normalize(direction) * radius_ + center_;
    }
    Vector2 ArbitraryPoint() const override {
        return center_;
    }

private:
    Vector2 center_;
    float radius_;
};



void DrawConvexHull(const ConvexHull& hull, const Matrix4x4& worldMat, unsigned int color) {
    std::vector<Vector2> vertices(hull.GetVertices().size());
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertices[i] = ToVector2(ToVector3(hull.GetVertices()[i]) * worldMat);
    }
    for (size_t i = 0; i < vertices.size(); ++i) {
        scene.DrawLine(vertices[i], vertices[(i + 1) % vertices.size()], color);
    }
}
extern std::vector<Vector2> pppp;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    std::srand(unsigned int(std::time(nullptr)));

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, (int)kWindowWidth, (int)kWindowHeight);
    Input* input = Input::GetInstance();

    ConvexHull hull1;
    //hull1.QuickHull(RandomPoints(100, 100, 10));
    hull1.QuickHull({ {50,50},{50,-50},{-50,-50},{-50,50} });
    Transformation transform1{};
    transform1.translate = { 100.0f, 50.0f, 0.0f };
    transform1.rotate = { 0.0f, 0.0f, 0.0f };
    transform1.scale = { 1.0f, 1.0f, 1.0f };

    ConvexHull hull2;
    //hull2.QuickHull(RandomPoints(100, 100, 10));
    hull2.QuickHull({ {50,50},{50,-50},{-50,-50},{-50,50} });
    Transformation transform2{};
    transform2.translate = { 0.0f, 0.0f, 0.0f };
    transform2.rotate = { 0.0f, 0.0f, 0.0f };
    transform2.scale = { 1.0f, 1.0f, 1.0f };

    Vector2 center{};
    float radius = 50.0f;

    unsigned int color = WHITE;

    // ウィンドウの×ボタンが押されるまでループ
    while (Novice::ProcessMessage() == 0) {
        // フレームの開始
        Novice::BeginFrame();

        ///
        /// ↓更新処理ここから
        ///



        {
            ImGui::SetNextWindowPos({ (float)kWindowWidth - 330.0f, 0.0f }, ImGuiCond_Once);
            ImGui::SetNextWindowSize({ 330.0f, 500.0f }, ImGuiCond_Once);
            ImGui::Begin("Window");

            ImGui::DragFloat2("transform1.translate", &transform1.translate.x);
            ImGui::DragFloatDegree("transform1.rotate", &transform1.rotate.z);
            ImGui::DragFloat2("transform1.scale", &transform1.scale.x, 0.1f);
            if (ImGui::Button("Regenerat1")) {
                hull1.QuickHull(RandomPoints(100, 100, 10));
            }

            ImGui::DragFloat2("transform2.translate", &transform2.translate.x);
            ImGui::DragFloatDegree("transform2.rotate", &transform2.rotate.z);
            ImGui::DragFloat2("transform2.scale", &transform2.scale.x, 0.1f);
            if (ImGui::Button("Regenerat2")) {
                hull2.QuickHull(RandomPoints(100, 100, 10));
            }
            ImGui::DragFloat2("center", &center.x);
            ImGui::DragFloat("radius", &radius, 0.1f);


            ImGui::End();
        }

        scene.Update();
        transform1.world = MakeAffineMatrix(transform1.scale, transform1.rotate, transform1.translate);
        transform2.world = MakeAffineMatrix(transform2.scale, transform2.rotate, transform2.translate);
        MeshCollider mc1;
        MeshCollider mc2;
        CircleCollider cc(center, radius);
        for (auto& v : hull1.GetVertices()) {
            mc1.AddVertex(Transform2D(v, transform1.world));
        }
        for (auto& v : hull2.GetVertices()) {
            mc2.AddVertex(Transform2D(v, transform2.world));
        }

        color = WHITE;
        std::vector<std::pair<Vector2, Vector2>> p;
        std::vector<std::pair<Vector2, Vector2>> p1;
     /*   if (GJK(mc1, mc2, p, p1)) {
            color = GREEN;
        }*/
        if (GJK(mc1, cc, p, p1)) {
            color = GREEN;
        }


        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        scene.DrawArea();

        Novice::DrawEllipse(100, 100, 100, 100, 0.0f, color, kFillModeSolid);
        DrawConvexHull(hull1, transform1.world, RED);
        scene.DrawCircle(center, radius, color);
        //DrawConvexHull(hull2, transform2.world, BLUE);
        //DrawConvexHull(ConvexHull::MinkowskiDifference(hull1, transform1.world, hull2, transform2.world), MakeIdentityMatrix(), color);


      /*  std::vector<unsigned int> colors = {
            0x000000FF,
            0xFF00FFFF,
            0xFFFF00FF,
            0x00FFFFFF,
            0xFF0000FF,
            0x00FF00FF,
            0x0000FFFF,
            0xFFFFFFFF,

        };

        mc1.Draw(RED);
        mc2.Draw(BLUE);
        auto iter = colors.begin();
        for (auto& v : p) {
            scene.DrawCircle(v.first, 2.0f, *iter);
            scene.DrawLine(Normalize(v.second) * 50, {}, *iter);
            ++iter;
        }
        iter = colors.begin();
        for (auto& v : p1) {
            scene.DrawCircle(v.first, 2.0f, *iter);
            scene.DrawCircle(v.second, 2.0f, *iter);
            ++iter;
        }*/


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

//void DrawCircle(const Vector2& c, float r, unsigned int color) {
//    Vector2 sc = c - cameraPos;
//    Novice::DrawEllipse(int(c.x), int(c.y), r, r, 0.0f, color, kFillModeSolid);
//}

//void DrawConvexPolygon(const ConvexPolygon& convexPolygon, unsigned int color, FillMode fillMode) {
//    
// 
//}

