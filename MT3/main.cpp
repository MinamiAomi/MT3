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

    Vector2 GetMouse() const {
        Input* input = Input::GetInstance();
        return ToVector2(ToVector3(input->GetMousePosition()) * Inverse(translateMatrix_));
    }

    void DrawArea() {
        float hw = width_ * 0.5f;
        float hh = height_ * 0.5f;
        Vector3 screenPos = Vector3{ -hw, -hh, 0.0f } *translateMatrix_;
        Novice::DrawBox(int(screenPos.x), int(screenPos.y), int(width_), -int(height_), 0.0f, 0x00440033, kFillModeSolid);
        DrawLine({ hw, 0.0f }, { -hw, 0.0f }, 0xFFFFFF33);
        DrawLine({ 0.0f, hh }, { 0.0f, -hh }, 0xFFFFFF33);
    }

    void DrawCircle(const Vector2& c, float r, unsigned int color, bool isWireFrame) {
        Vector3 screenPos = Vector3{ c.x, c.y, 0.0f } *translateMatrix_;
        Novice::DrawEllipse(int(screenPos.x), int(screenPos.y), int(r), int(r), 0.0f, color, isWireFrame ? kFillModeWireFrame : kFillModeSolid);
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
    MeshCollider(const std::vector<Vector2>& vertices, const Matrix4x4& m) {
        aabb_.min = aabb_.max = Transform2D(vertices[0], m);
        for (auto& v : vertices) {
            vertices_.emplace_back(Transform2D(v, m));
            aabb_.min.x = std::min(aabb_.min.x, vertices_.back().x);
            aabb_.min.y = std::min(aabb_.min.y, vertices_.back().y);
            aabb_.max.x = std::max(aabb_.max.x, vertices_.back().x);
            aabb_.max.y = std::max(aabb_.max.y, vertices_.back().y);
        }
    }
    Vector2 FindFurthestPoint(const Vector2& direction) const override {
        std::vector<Vector2>::const_iterator iter = vertices_.begin();
        std::vector<Vector2>::const_iterator findPoint = iter;
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
    const std::vector<Vector2>& GetVertices() const { return vertices_; }
private:
    std::vector<Vector2> vertices_;
};

class CircleCollider :
    public CollisionShape {
public:

    CircleCollider(const Vector2& c, float r) : center_(c), radius_(r) {
        aabb_.min = { center_.x - radius_, center_.y - radius_ };
        aabb_.max = { center_.x + radius_, center_.y + radius_ };
    }
    Vector2 FindFurthestPoint(const Vector2& direction) const override {
        return Normalize(direction) * radius_ + center_;
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
    hull2.QuickHull(RandomPoints(100, 100, 10));
    Transformation transform2{};
    transform2.translate = { 0.0f, 0.0f, 0.0f };
    transform2.rotate = { 0.0f, 0.0f, 0.0f };
    transform2.scale = { 1.0f, 1.0f, 1.0f };

    Vector2 center{};
    float radius = 50.0f;

    unsigned int color0 = WHITE;
    unsigned int color1 = WHITE;
    unsigned int color2 = WHITE;

    std::vector<Vector2> terrain = {
        {-250.0f, 500.0f},
        {-250, -130},
        {-200, -120},
        {-150, -140},
        {-100, -110},
        {-50, -100},
        {   0, -120},
        {  50, -130},
        { 100, -150},
        { 150, -120},
        { 200, -110},
        { 250, -150},
        {250.0f, 500.0f},
     {-250.0f, 500.0f}, };
    std::vector<MeshCollider> tmc;
    for (size_t i = 0; i < terrain.size() - 1; ++i) {
        tmc.emplace_back(std::vector<Vector2>({ terrain[i], terrain[i + 1] }), MakeIdentityMatrix());
    }

    Vector2 vel1{};
    Vector2 vel2{};
    Vector2 vel3{};

    // ウィンドウの×ボタンが押されるまでループ
    while (Novice::ProcessMessage() == 0) {
        // フレームの開始
        Novice::BeginFrame();

        ///
        /// ↓更新処理ここから
        ///



        {
            auto& io = ImGui::GetIO();
            ImGui::SetNextWindowPos({ (float)kWindowWidth - 330.0f, 0.0f }, ImGuiCond_Once);
            ImGui::SetNextWindowSize({ 330.0f, 500.0f }, ImGuiCond_Once);
            ImGui::Begin("Window");

            ImGui::Text("framerate %f", io.Framerate);
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
            ImGui::DragFloat2("circle center", &center.x);
            ImGui::DragFloat("circle radius", &radius, 0.1f);


            ImGui::End();
        }

        scene.Update();

        if (input->PushKey(DIK_SPACE)) {
            auto rp = []() {
                Vector2 v{};
                v.x = Math::Lerp(10.0f, -10.0f, std::rand() / float(RAND_MAX));
                v.y = Math::Lerp(10.0f, 20.0f, std::rand() / float(RAND_MAX));
                return v;
            };
            vel1 = rp();
            vel2 = rp();
            vel3 = rp();
        }

        vel1 += -Vector2UnitY * 0.2f;
        vel2 += -Vector2UnitY * 0.2f;
        vel3 += -Vector2UnitY * 0.2f;
        transform1.translate += ToVector3(vel1);
        transform2.translate += ToVector3(vel2);
        center += vel3;

        if (input->IsPressMouse(2)) {
            if (input->PushKey(DIK_1)) {
                transform1.translate = ToVector3(scene.GetMouse());
                vel1 = {};
            }
            else if (input->PushKey(DIK_2)) {
                transform2.translate = ToVector3(scene.GetMouse());
                vel2 = {};
            }
            else if (input->PushKey(DIK_3)) {
                center = scene.GetMouse();
                vel3 = {};
            }
        }
        transform1.world = MakeAffineMatrix(transform1.scale, transform1.rotate, transform1.translate);
        transform2.world = MakeAffineMatrix(transform2.scale, transform2.rotate, transform2.translate);


        MeshCollider mc1(hull1.GetVertices(), transform1.world);
        MeshCollider mc2(hull2.GetVertices(), transform2.world);
        CircleCollider cc(center, radius);


        Simplex simplex0;
        Simplex simplex1;
        Simplex simplex2;
        color0 = WHITE;
        color1 = WHITE;
        color2 = WHITE;

        if (NarrowPhaseAABB(mc1, mc2)) {
            if (GJK(mc1, mc2, &simplex0)) {
                Vector2 bounce = EPA(simplex0, mc1, mc2);
                transform1.translate -= ToVector3(bounce * 0.5f);
                transform2.translate += ToVector3(bounce * 0.5f);
                color0 = RED;
                color1 = BLUE;
                vel1 = Reflecte(vel1, Normalize(-bounce)) * 0.8f * 0.8f;
                vel2 = Reflecte(vel2, Normalize(bounce)) * 0.8f * 0.8f;
            }
        }
        if (NarrowPhaseAABB(mc1, cc)) {
            if (GJK(mc1, cc, &simplex1)) {
                Vector2 bounce = EPA(simplex1, mc1, cc);
                transform1.translate -= ToVector3(bounce * 0.5f);
                center += bounce * 0.5f;
                color0 = RED;
                color2 = GREEN;
                vel1 = Reflecte(vel1, Normalize(-bounce)) * 0.8f * 0.8f;
                vel3 = Reflecte(vel3, Normalize(bounce)) * 0.8f * 0.8f;
            }
        }
        if (NarrowPhaseAABB(mc2, cc)) {
            if (GJK(mc2, cc, &simplex2)) {
                Vector2 bounce = EPA(simplex2, mc2, cc);
                transform2.translate -= ToVector3(bounce * 0.5f);
                center += bounce * 0.5f;
                color1 = BLUE;
                color2 = GREEN;
                vel2 = Reflecte(vel2, Normalize(-bounce)) * 0.8f * 0.8f;
                vel3 = Reflecte(vel3, Normalize(bounce)) * 0.8f * 0.8f;
            }
        }

        Vector2 bou1{};
        size_t hitCount1 = 0;
        Vector2 bou2{};
        size_t hitCount2 = 0;
        Vector2 bou3{};
        size_t hitCount3 = 0;
        for (auto& t : tmc) {
            if (NarrowPhaseAABB(mc1, t)) {
                Simplex s;
                if (GJK(mc1, t, &s)) {
                    bou1 += EPA(s, mc1, t);
                    hitCount1++;
                    color0 = RED;
                }
            }
            if (NarrowPhaseAABB(mc2, t)) {
                Simplex s;
                if (GJK(mc2, t, &s)) {
                    bou2 += EPA(s, mc2, t);
                    hitCount2++;
                    color1 = BLUE;
                }
            }
            if (NarrowPhaseAABB(cc, t)) {
                Simplex s;
                if (GJK(cc, t, &s)) {
                    bou3 += EPA(s, cc, t);
                    hitCount3++;
                    color2 = GREEN;
                }
            }
        }
        if (hitCount1 > 0) {
            Vector2 v = bou1 * -(1.0f / hitCount1);
            transform1.translate += ToVector3(v);
            vel1 = Reflecte(vel1, Normalize(v)) * 0.8f;
        }
        if (hitCount2 > 0) {
            Vector2 v = bou2 * -(1.0f / hitCount2);
            transform2.translate += ToVector3(v);
            vel2 = Reflecte(vel2, Normalize(v)) * 0.8f;
        }
        if (hitCount3 > 0) {
            Vector2 v = bou3 * -(1.0f / hitCount3);
            center += v;
            vel3 = Reflecte(vel3, Normalize(v)) * 0.8f;
        }

        transform1.world = MakeAffineMatrix(transform1.scale, transform1.rotate, transform1.translate);
        transform2.world = MakeAffineMatrix(transform2.scale, transform2.rotate, transform2.translate);

        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        scene.DrawArea();

        //Novice::DrawEllipse(100, 100, 100, 100, 0.0f, color, kFillModeSolid);
        DrawConvexHull(hull1, transform1.world, color0);
        DrawConvexHull(hull2, transform2.world, color1);
        scene.DrawCircle(center, radius, color2, true);

        for (size_t i = 0; i < terrain.size() - 1; ++i) {
            scene.DrawLine(terrain[i], terrain[i + 1], WHITE);
            scene.DrawCircle(terrain[i], 3, WHITE, false);

        }

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

