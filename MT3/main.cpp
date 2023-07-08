#include <Novice.h>
#include <Input.h>
#include <cstdint>
#include <ImGuiManager.h>
#include "MyMath.h"
#include "ReneringPipeline.h"
#include "Collision.h"
#include "PrintUtils.h"
#include "ConvexPolygon.h"

const char kWindowTitle[] = "LE2A_19_ミナミアオミ";
const uint32_t kWindowWidth = 1280;
const uint32_t kWindowHeight = 720;

void DrawConvexPolygon(const ConvexPolygon& convexPolygon, unsigned int color, FillMode fillMode);

Vector2 cameraPos;
void DrawCircle(const Vector2& c, float r, unsigned int color);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    std::srand(unsigned int(std::time(nullptr)));

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, (int)kWindowWidth, (int)kWindowHeight);
    Input* input = Input::GetInstance();

    std::vector<Vector2> vertices(50);
    for (auto& v : vertices) {
        v.x = Math::Lerp()
    }

    Vector2 centroid{};
    for (const auto& v : vertices) {
        centroid += v;
    }
    centroid *= 1.0f / vertices.size();

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


            ImGui::End();
        }



        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        for (auto& v : vertices) {
            Novice::DrawEllipse(int(v.x), int(v.y), 10, 10, 0.0f, WHITE, kFillModeSolid);
        }
        //Novice::DrawEllipse(int(centroid.x), int(centroid.y), 10, 10, 0.0f, BLUE, kFillModeSolid);

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

void DrawCircle(const Vector2& c, float r, unsigned int color) {
    Vector2 sc = c - cameraPos;
    Novice::DrawEllipse(int(c.x), int(c.y), r, r, 0.0f, color, kFillModeSolid);
}

//void DrawConvexPolygon(const ConvexPolygon& convexPolygon, unsigned int color, FillMode fillMode) {
//    
// 
//}
