#pragma once

#include <array>
#include <cstdint>
#include <initializer_list>
#include <vector>

#include "MyMath.h"

struct AABB {
    Vector2 min, max;
};

class CollisionShape {
public:
    virtual ~CollisionShape() {}
    virtual Vector2 FindFurthestPoint(const Vector2& direction) const = 0;
    
    const AABB& GetAABB() const { return aabb_; }
protected:
    AABB aabb_{};
};

class Simplex {
public:
    Simplex();
    Simplex& operator=(const std::initializer_list<Vector2>& list);
    void push_front(const Vector2& point);

    Vector2& operator[](size_t i) { return points_[i]; }
    uint32_t size() const { return size_; }

    auto begin() const { return points_.begin(); }
    auto end() const { return points_.end(); }

private:
    std::array<Vector2, 3> points_;
    uint32_t size_;
};

bool NarrowPhaseAABB(const CollisionShape& collisionShape1, const CollisionShape& collisionShape2);
bool GJK(const CollisionShape& collisionShape1, const CollisionShape& collisionShape2, Simplex* simplex);
Vector2 EPA(const Simplex& simplex, const CollisionShape& collisionShape1, const CollisionShape& collisionShape2);

bool CollisionTerrain(const std::vector<Vector2>& terrain, const Vector2& up, const CollisionShape& collisionShape, std::vector<size_t>& index);