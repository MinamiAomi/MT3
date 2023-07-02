#include "BVH.h"

void BVHTree::RegisterCollider(Collider* collider) {
    colliders_.emplace_back(collider);
}

void BVHTree::Build() {
    const size_t internalNodeCount = colliders_.size() - 1;
    const size_t leafNodeCount = colliders_.size();

    nodes_.resize(internalNodeCount);
    
}
