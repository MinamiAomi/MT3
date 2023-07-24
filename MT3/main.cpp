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



struct TestObj {
    ConvexHull hull;
    std::vector<Vector2> points = RandomPoints(100.0f, 100.0f, 10);
    Vector2 translate = { 0.0f,0.0f };
    bool isDrawPoints = false;

    Vector2 FindFurthestPoint(const Vector2& direction) const {
        std::vector<Vector2>::const_iterator iter = hull.GetVertices().begin();
        std::vector<Vector2>::const_iterator findPoint = iter;
        float maxDistance = Dot(*iter++, direction);
        for (; iter != hull.GetVertices().end(); ++iter) {
            float d = Dot(*iter, direction);
            if (d > maxDistance) {
                maxDistance = d;
                findPoint = iter;
            }
        }
        return *findPoint + translate;
    }

    void Draw(unsigned int color) {
        if (isDrawPoints) {
              for (const auto& p : points) {
                  scene.DrawCircle(p + translate, 2, color, false);
              }
        }
        std::vector<Vector2> vertices(hull.GetVertices().size());
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i] = hull.GetVertices()[i] + translate;
            scene.DrawCircle(vertices[i], 2, color, false);
        }
        for (size_t i = 0; i < vertices.size(); ++i) {
            scene.DrawLine(vertices[i], vertices[(i + 1) % vertices.size()], color);
        }
    }

};

void MinDiff(const TestObj& o1, const TestObj& o2, TestObj& out) {
    std::vector<Vector2> v1(o1.hull.GetVertices().size());
    for (size_t i = 0; i < v1.size(); i++) {
        v1[i] = o1.hull.GetVertices()[i] + o1.translate;
    }
    std::vector<Vector2> v2(o2.hull.GetVertices().size());
    for (size_t i = 0; i < v2.size(); i++) {
        v2[i] = o2.hull.GetVertices()[i] + o2.translate;
    }

    out.points.resize(v1.size() * v2.size());
    auto p = out.points.begin();
    for (auto& p1 : v1) {
        for (auto& p2 : v2) {
            *p = p1 - p2;
            ++p;
        }
    }
    out.hull.QuickHull(out.points);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    std::srand(unsigned int(std::time(nullptr)));

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, (int)kWindowWidth, (int)kWindowHeight);
    Input* input = Input::GetInstance();

    TestObj obj1;
    obj1.hull.QuickHull(obj1.points);
    obj1.translate = { 125.0f,125.0f };


    TestObj obj2;
    obj2.hull.QuickHull(obj2.points);
    obj2.translate = { -125.0f,125.0f };

    TestObj obj3;
    obj3.hull.QuickHull(obj3.points);
    obj3.translate = { -125.0f,-125.0f };

    TestObj obj4;
    obj4.hull.QuickHull(obj4.points);
    obj4.translate = { 125.0f,-125.0f };

    TestObj minDiff;
    MinDiff(obj1, obj2, minDiff);

    bool edit = true;
    bool start = false;
    bool step = false;

    std::vector<Vector2> simplex;
    Vector2 direction{ 1,0 };
    Vector2 support{ 0,0 };


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

            if (ImGui::Checkbox("Edit", &edit)) {
                if (edit) {

                }
                else {
                    start = false;
                    step = false;
                    direction = { 1,0 };
                }
            }


            if (edit) {
                ImGui::DragFloat2("Obj1 translate", &obj1.translate.x);
                if (ImGui::Button("Obj1 Regenerat")) {
                    obj1.points = RandomPoints(100.0f, 100.0f, 20);
                    obj1.hull.QuickHull(obj1.points);
                }
                ImGui::DragFloat2("Obj2 translate", &obj2.translate.x);
                if (ImGui::Button("Obj2 Regenerat")) {
                    obj2.points = RandomPoints(100.0f, 100.0f, 20);
                    obj2.hull.QuickHull(obj2.points);
                }
                ImGui::DragFloat2("Obj3 translate", &obj3.translate.x);
                if (ImGui::Button("Obj3 Regenerat")) {
                    obj3.points = RandomPoints(100.0f, 100.0f, 20);
                    obj3.hull.QuickHull(obj3.points);
                }
                ImGui::DragFloat2("Obj4 translate", &obj4.translate.x);
                if (ImGui::Button("Obj4 Regenerat")) {
                    obj4.points = RandomPoints(100.0f, 100.0f, 20);
                    obj4.hull.QuickHull(obj4.points);
                }
            }
            else {
                if (start) {
                    step = ImGui::Button("Step");
                    ImGui::DragFloat2("Direction", &direction.x);

                }
                else {
                    start = ImGui::Button("Start");
                    if (start) {
                        direction = { 1,0 };
                        support = minDiff.FindFurthestPoint(direction);
                        simplex.emplace_back(support);
                        direction = -support;
                    }
                }
            }


            ImGui::End();
        }

        scene.Update();
        MinDiff(obj1, obj2, minDiff);

        if (step) {
          


        }


        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        scene.DrawArea();

        obj1.Draw(RED);
        obj2.Draw(BLUE);
        obj3.Draw(GREEN);
        obj4.Draw(0xFFFF00FF);
        //minDiff.Draw(0xFF00FFFF);

        if (!edit) {
            if (simplex.size() == 0) {
                scene.DrawLine({}, Normalize(direction) * 100, BLACK);
            }
            else {
                scene.DrawLine(simplex[0], simplex[0] + Normalize(direction) * 100, BLACK);
            }
            scene.DrawCircle(support, 3, RED, false);
        }
        Vector2 mouse = Normalize(scene.GetMouse()) * 50;

        scene.DrawCircle(obj1.FindFurthestPoint(mouse), 3, BLACK, false);
        scene.DrawCircle(obj2.FindFurthestPoint(mouse), 3, BLACK, false);
        scene.DrawCircle(obj3.FindFurthestPoint(mouse), 3, BLACK, false);
        scene.DrawCircle(obj4.FindFurthestPoint(mouse), 3, BLACK, false);
        //scene.DrawCircle(minDiff.FindFurthestPoint(mouse), 3, BLACK, false);
        scene.DrawLine({}, mouse * 2, BLACK);

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

