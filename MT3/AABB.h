#pragma once

#include "MyMath.h"

class AABB {
public:
    AABB() : min({}), max({}) {}
    AABB(const Vector2& min, const Vector2& max) : min(min), max(max) {}
    explicit AABB(const Vector2& minmax) : min(minmax), max(minmax) {}

    Vector2 GetSize() const { return max - min; }
    Vector2 GetCenter() const { return (min + max) * 0.5f; }
    float GetCenterX() const { return (min.x + max.x) * 0.5f; }
    float GetCenterY() const { return (min.y + max.y) * 0.5f; }

    static AABB Merge(const AABB& aabb1, const AABB& aabb2) {
        AABB aabb;
        aabb.min = Min(aabb1.min, aabb2.min);
        aabb.max = Min(aabb1.max, aabb2.max);
        return aabb;
    }
    static bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
        return
            (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
            (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y);
    }

    
    Vector2 min, max;
};