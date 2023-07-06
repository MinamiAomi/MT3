#include "BVH.h"

#include <stack>
#include <span>

void BVHTree::RegisterCollider(Collider* collider) {
    colliders_.emplace_back(collider);
}

void BVHTree::Build() {
    // ノードの数は葉の数-1
    const size_t leafNodeCount = colliders_.size();
    const size_t internalNodeCount = leafNodeCount - 1;

    nodes_.resize(internalNodeCount);

    // ソートする軸のインデックス
    size_t axisIndex = 0;
    // AABBの中心座標でソート
    auto Compare = [&](Collider* lhs, Collider* rhs) {
        return Math::Mid(lhs->GetAABB().min[axisIndex], lhs->GetAABB().max[axisIndex]) < 
            Math::Mid(rhs->GetAABB().min[axisIndex], rhs->GetAABB().max[axisIndex]);
    };

    std::stack<std::span<Collider*>> stack;
    // スタックにすべてのコライダーを追加
    stack.push(std::span(colliders_));
    int32_t nodeIndex = 0;

    while (true) {
        // スタックからコライダーを取得
        std::span colliders = stack.top();
        stack.pop();

        auto& node = nodes_[nodeIndex];

        // ノードのAABBを計算
        node.aabb = colliders[0]->GetAABB();
        if (colliders.size() > 1) {
            for (auto iter = colliders.begin(); iter != colliders.end(); ++iter) {
                node.aabb = AABB::Merge(node.aabb, (*iter)->GetAABB());
            }
        }

        if (colliders.size() <= 1) {
            nodes_[nodeIndex].aabb = colliders[0]->GetAABB();
            nodes_[nodeIndex].colliderIndex = colliders.data() - colliders_.data();
        }

        // ソートする軸を決定する
        auto Compare = [](Collider* lhs, Collider* rhs) {
            return lhs->GetAABB().GetCenterX() < rhs->GetAABB().GetCenterX();
        };
        
        auto size = node.aabb.max - node.aabb.min;
        axisIndex = size.x > size.y ? 0 : 1;
        // 軸ソート
        std::sort(colliders.begin(), colliders.end(), Compare);

        size_t splitIndex = colliders.size() * 0.5f;
        stack.push(std::span(colliders.begin(), colliders.begin() + splitIndex));
        stack.push(std::span(colliders.begin() + splitIndex, colliders.end()));


        ++nodeIndex;
    }

}
