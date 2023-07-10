#pragma once

#include <array>
#include <cstdint>
#include <initializer_list>
#include <vector>

#include "MyMath.h"

class CollisionShape {
public:
    virtual ~CollisionShape() {}
    virtual Vector2 FindFurthestPoint(const Vector2& direction) const = 0;
    virtual Vector2 ArbitraryPoint() const = 0;
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

bool GJK(const CollisionShape& collisionShape1, const CollisionShape& collisionShape2, std::vector<std::pair<Vector2, Vector2>>& p, std::vector<std::pair<Vector2, Vector2>>& p1);
