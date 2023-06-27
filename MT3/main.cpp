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

float areaHalfWidth = 300.0f;
float areaHalfHeight = 300.0f;

bool a = 0;

struct AABB {
    Vector2 min;
    Vector2 max;
    static bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
        return
            (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
            (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y);
    }
};

class Object {
public:
    Vector2 center;
    float radius;
    uint32_t color;
    Vector2 velocity;

    void Initalize() {
        radius = Math::Lerp(5.0f, 10.0f, (rand() / float(RAND_MAX)));
        float rangeWidth = areaHalfWidth - radius;
        float rangeHeight = areaHalfHeight - radius;;
        center.x = Math::Lerp(-rangeWidth, rangeWidth, (rand() / float(RAND_MAX)));
        center.y = Math::Lerp(-rangeHeight, rangeHeight, (rand() / float(RAND_MAX)));
        color = WHITE;
        float rad = (rand() / float(RAND_MAX)) * Math::TwoPi;
        velocity.x = std::cos(rad);
        velocity.y = std::sin(rad);
        UpdateAABB();
    }
    void Update() {
        center += velocity;
        color = WHITE;
        if (center.x < -areaHalfWidth + radius) {
            center.x = -areaHalfWidth + radius;
            velocity.x = -velocity.x;
        }
        if (center.x > areaHalfWidth - radius) {
            center.x = areaHalfWidth - radius;
            velocity.x = -velocity.x;
        }
        if (center.y < -areaHalfHeight + radius) {
            center.y = -areaHalfHeight + radius;
            velocity.y = -velocity.y;
        }
        if (center.y > areaHalfHeight - radius) {
            center.y = areaHalfHeight - radius;
            velocity.y = -velocity.y;
        }
        UpdateAABB();
    }
    void Draw(const Matrix4x4& mat) {
        Vector3 screenPos = Vector3{ center.x,center.y,0.0f } *mat;
        Novice::DrawEllipse(int(screenPos.x), int(screenPos.y), int(radius), int(radius), 0.0f, color, kFillModeSolid);
    }
    const AABB& GetAABB() { return aabb_; }

    static bool IsCollision(const Object& object1, const Object& object2) {
        return LengthSquare(object2.center - object1.center) <= (object1.radius + object2.radius) * (object1.radius + object2.radius);
    }
private:
    void UpdateAABB() {
        aabb_ = {
           {center.x - radius, center.y - radius},
           {center.x + radius, center.y + radius} };
    }
    AABB aabb_;
};

struct BVHNode {
    AABB aabb;
    std::unique_ptr<BVHNode> left;
    std::unique_ptr<BVHNode> right;
    std::shared_ptr<Object> object;
};

class BVH {
public:
    void Clear() {
        objects_.clear();
        leafs_.clear();
        root_.reset();
    }
    void RegisterObject(const std::shared_ptr<Object>& object) { objects_.emplace_back(object); }

    void Build() {
        buildNodeCallCount = 0;
        a = true;
        leafs_.clear();
        root_ = BuildNodeX(objects_);
    }

    void Traversal() {
        traversalNodeCallCount = 0;
        objectIsCollisionCallCount = 0;
        for (auto leaf : leafs_) {
            TraversalNode(root_.get(), leaf);
        }

    }

    void DrawNodeAABB(const Matrix4x4& mat) {
        DrawNode(root_.get(), mat);
    }
    size_t buildNodeCallCount = 0;
    size_t traversalNodeCallCount = 0;
    size_t objectIsCollisionCallCount = 0;

    size_t GetObjectCount() { return objects_.size(); }
private:
    std::unique_ptr<BVHNode> BuildNodeX(std::vector<std::shared_ptr<Object>>& objects);
    std::unique_ptr<BVHNode> BuildNodeY(std::vector<std::shared_ptr<Object>>& objects);
    void DrawNode(BVHNode* node, const Matrix4x4& mat);
    void TraversalNode(BVHNode* node, BVHNode* leaf);

    std::vector<std::shared_ptr<Object>> objects_;
    std::unique_ptr<BVHNode> root_;
    std::vector<BVHNode*> leafs_;
};

class Scene {
public:
    void GenerateObjects(size_t objectCount) {
        objects_.clear();
        for (int i = 0; i < objectCount; i++) {
            auto& object = objects_.emplace_back(std::make_shared<Object>());
            object->Initalize();
        }
    }

    void Update() {
        Input* input = Input::GetInstance();
        if (input->IsPressMouse(1)) {
            auto mouseMove = input->GetMouseMove();
            cameraPosition_.x -= float(mouseMove.lX);
            cameraPosition_.y += float(mouseMove.lY);
        }
        if (!stop) {
            for (auto& object : objects_) {
                object->Update();
            }
        }
    }

    void CheckCollisionForBvh() {
        if (!stop) {
            bvh_.Clear();
            for (auto& object : objects_) {
                bvh_.RegisterObject(object);
            }
            bvh_.Build();
            bvh_.Traversal();
        }
    }

    void ImGuiDraw() {
        ImGui::Text("ObjectCount : %zu", bvh_.GetObjectCount());
        ImGui::Text("BuildNodeCollCount : %zu", bvh_.buildNodeCallCount);
        ImGui::Text("TraversalNodeCollCount : %zu", bvh_.traversalNodeCallCount);
        ImGui::Text("LeafIsCollisionCount : %zu", bvh_.objectIsCollisionCallCount);
        ImGui::Checkbox("Show BVH", &showBVH);
        ImGui::Checkbox("Stop", &stop);
        // ImGui::Text(" : %zu", bvh_.GetObjectCount() * bvh_.GetObjectCount());
    }

    void Draw() {
        translateMatrix_ = MakeTranslateMatrix(-cameraPosition_) * screenOffsetMatrix_;

        float width = areaHalfWidth * 2.0f;
        float height = areaHalfHeight * 2.0f;
        Vector3 screenPos = Vector3{ -areaHalfWidth,-areaHalfHeight,0.0f } *translateMatrix_;
        Novice::DrawBox(int(screenPos.x), int(screenPos.y), int(width), -int(height), 0.0f, 0x00440033, kFillModeSolid);

        for (auto& object : objects_) {
            object->Draw(translateMatrix_);
        }
        if (showBVH) {
            bvh_.DrawNodeAABB(translateMatrix_);
        }
    }

private:
    std::vector<std::shared_ptr<Object>> objects_;
    BVH bvh_;

    Vector3 cameraPosition_{ 0.0f, 0.0f, 0.0f };
    Matrix4x4 translateMatrix_ = MakeTranslateMatrix(-cameraPosition_);
    Matrix4x4 screenOffsetMatrix_ = MakeScaleMatrix({ 1.0f,-1.0f,1.0f }) * MakeTranslateMatrix({ kWindowWidth * 0.5f, kWindowHeight * 0.5f, 0.0f });

    bool stop;
    bool showBVH = true;
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    srand(unsigned int(time(nullptr)));

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, (int)kWindowWidth, (int)kWindowHeight);
    Input* input = Input::GetInstance();

    Scene scene;
    size_t objectCount = 10;
    scene.GenerateObjects(objectCount);

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
            ImGui::Text("Framerate : %f", io.Framerate);

            int buf = int(objectCount);
            ImGui::DragInt("ObjectCount", &buf, 1, 0, 500);
            objectCount = size_t(buf);
            if (ImGui::Button("ResetObjects")) {
                scene.GenerateObjects(objectCount);
            }
            scene.ImGuiDraw();
            ImGui::End();
        }

        scene.Update();
        scene.CheckCollisionForBvh();


        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        scene.Draw();

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

std::unique_ptr<BVHNode> BVH::BuildNodeX(std::vector<std::shared_ptr<Object>>& objects) {
    buildNodeCallCount++;
    std::unique_ptr<BVHNode> node = std::make_unique<BVHNode>();

    if (objects.empty()) {
        return node;
    }

    auto objIter = objects.begin();
    node->aabb = (*objIter)->GetAABB();
    for (; objIter != objects.end(); ++objIter) {
        AABB objAABB = (*objIter)->GetAABB();
        if (objAABB.min.x < node->aabb.min.x) { node->aabb.min.x = objAABB.min.x; }
        if (objAABB.min.y < node->aabb.min.y) { node->aabb.min.y = objAABB.min.y; }
        if (objAABB.max.x > node->aabb.max.x) { node->aabb.max.x = objAABB.max.x; }
        if (objAABB.max.y > node->aabb.max.y) { node->aabb.max.y = objAABB.max.y; }
    }

    if (objects.size() > 1) {
        std::sort(objects.begin(), objects.end(),
            [](const std::shared_ptr<Object>& object1, const std::shared_ptr<Object>& object2) {
                return object1->center.x > object2->center.x;
            });
        size_t splitIndex = objects.size() / 2;
        if (a) {
            (*(objects.begin() + (splitIndex - 1)))->color = BLUE;
            (*(objects.begin() + (splitIndex)))->color = GREEN;
            a = false;
        }
        std::vector<std::shared_ptr<Object>> leftSubset(objects.begin(), objects.begin() + splitIndex);
        std::vector<std::shared_ptr<Object>> rightSubset(objects.begin() + splitIndex, objects.end());

        node->left = BuildNodeY(leftSubset);
        node->right = BuildNodeY(rightSubset);
    }
    else {
        node->object = objects[0];
        leafs_.emplace_back(node.get());
    }

    return node;
}

std::unique_ptr<BVHNode> BVH::BuildNodeY(std::vector<std::shared_ptr<Object>>& objects) {
    buildNodeCallCount++;
    std::unique_ptr<BVHNode> node = std::make_unique<BVHNode>();

    if (objects.empty()) {
        return node;
    }

    auto objIter = objects.begin();
    node->aabb = (*objIter)->GetAABB();
    for (; objIter != objects.end(); ++objIter) {
        AABB objAABB = (*objIter)->GetAABB();
        if (objAABB.min.x < node->aabb.min.x) { node->aabb.min.x = objAABB.min.x; }
        if (objAABB.min.y < node->aabb.min.y) { node->aabb.min.y = objAABB.min.y; }
        if (objAABB.max.x > node->aabb.max.x) { node->aabb.max.x = objAABB.max.x; }
        if (objAABB.max.y > node->aabb.max.y) { node->aabb.max.y = objAABB.max.y; }
    }

    if (objects.size() > 1) {
        std::sort(objects.begin(), objects.end(),
            [](const std::shared_ptr<Object>& object1, const std::shared_ptr<Object>& object2) {
                return object1->center.y > object2->center.y;
            });

        size_t splitIndex = objects.size() / 2;
        std::vector<std::shared_ptr<Object>> leftSubset(objects.begin(), objects.begin() + splitIndex);
        std::vector<std::shared_ptr<Object>> rightSubset(objects.begin() + splitIndex, objects.end());

        node->left = BuildNodeX(leftSubset);
        node->right = BuildNodeX(rightSubset);
    }
    else {
        node->object = objects[0];
        leafs_.emplace_back(node.get());
    }

    return node;
}

void BVH::DrawNode(BVHNode* node, const Matrix4x4& mat) {
    if (node->right) {
        DrawNode(node->right.get(), mat);
    }
    if (node->left) {
        DrawNode(node->left.get(), mat);
    }
    float width = node->aabb.max.x - node->aabb.min.x;
    float height = node->aabb.max.y - node->aabb.min.y;
    Vector3 screenPos = Vector3{ node->aabb.min.x,node->aabb.min.y,0.0f } *mat;
    Novice::DrawBox(int(screenPos.x), int(screenPos.y), int(width), -int(height), 0.0f, WHITE, kFillModeWireFrame);
}

void BVH::TraversalNode(BVHNode* node, BVHNode* leaf) {
    traversalNodeCallCount++;
    // ノードと衝突している
    if (AABB::IsCollision(node->aabb, leaf->aabb)) {
        // ノードが葉ノード
        if (node->object && node->object != leaf->object) {
            objectIsCollisionCallCount++;
            if (Object::IsCollision(*node->object, *leaf->object)) {
                if (leaf->object->color == WHITE) {
                    leaf->object->color = RED;
                }
                else if (leaf->object->color == BLUE) {
                    leaf->object->color = 0xFF00FFFF;
                }
                else if (leaf->object->color == GREEN) {
                    leaf->object->color = 0xFFFF00FF;
                }
                return;
            }
        }
        if (node->left) {
            TraversalNode(node->left.get(), leaf);
        }
        if (node->right) {
            TraversalNode(node->right.get(), leaf);
        }
    }
}
