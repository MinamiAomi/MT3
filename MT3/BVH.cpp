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

    std::stack<std::span<Collider*>> stack;
    // スタックにすべてのコライダーを追加
    stack.push(std::span(colliders_));
    int32_t nodeIndex = 0;

    while (true) {
        // スタックからコライダーを取得
        std::span colliders = std::move(stack.top());
        stack.pop();

        if (colliders.size() <= 1) {
            nodes_[nodeIndex].aabb = colliders[0]->GetAABB();
            nodes_[nodeIndex].colliderIndex = colliders.data() - colliders_.data();
        }

        // ノードのAABBを計算
        for (auto collider : colliders) {
            nodes_[nodeIndex].aabb = AABB::Merge(nodes_[nodeIndex].aabb, collider->GetAABB());
        }

        auto Compare = [](Collider* lhs, Collider* rhs) {
            return lhs->GetAABB().GetCenterX() < rhs->GetAABB().GetCenterX();
        };
        // 軸ソート
        std::sort(colliders.begin(), colliders.end(), Compare);

        stack.push(std::span(colliders.begin(), colliders.begin() + colliders.size() / 2));
        stack.push(std::span(colliders.begin() + colliders.size() / 2, colliders.end()));

        ++nodeIndex;
    }

}
