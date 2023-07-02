#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "MyMath.h"
#include "AABB.h"

class Collider {
public:
    Collider(void* owner) : owner_(owner) {}

    void SetAABB(const AABB& aabb) { aabb_ = aabb; }
    void SetCollisionCallBack(const std::function<void(void)>& collisionCallBack) { collisionCallBack_ = collisionCallBack; }

    template<class T>
    const T* GetOwner() const { return static_cast<T*>(owner_); }
    const AABB& GetAABB() const { return aabb_; }

private:
    void* owner_;
    AABB aabb_;
    std::function<void(void)> collisionCallBack_;
};

class BVHObject {
    void* bvhObject;
};

class BVHTree {
public:
    void RegisterCollider(Collider* collider);

    void Build();
    void Traversal();

private:
    int32_t AllocateNode();

    struct BVHNode {
        AABB aabb;
        int32_t parent;
        int32_t child1;
        int32_t child2;
        int32_t colliderIndex;
        bool isLeaf;

        BVHNode() : parent(-1), child1(-1), child2(-1), colliderIndex(-1), isLeaf(false) {}
    };

    std::vector<BVHNode> nodes_;
    std::vector<Collider*> colliders_;
    int32_t rootIndex_;
};