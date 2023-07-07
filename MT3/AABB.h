#pragma once

#include "MyMath.h"

#include <limits>

class AABB {
public:
    AABB() :
        min({ std::numeric_limits<float>::max(), std::numeric_limits<float>::max() }),
        max({ std::numeric_limits<float>::min(), std::numeric_limits<float>::min() }) {}
    AABB(const Vector2& min, const Vector2& max) : min(min), max(max) {}
    AABB(const Vector2& v1, const Vector2& v2, const Vector2& v3) : min(v1), max(v1) {
        Include(v2);
        Include(v3);
    }
    explicit AABB(const Vector2& minmax) : min(minmax), max(minmax) {}

    void Include(const AABB& aabb) {
        min = Min(min, aabb.min);
        max = Max(max, aabb.max);
    }
    void Include(const Vector2& point) {
        min = Min(min, point);
        max = Max(max, point);
    }

    Vector2 Extent() const { return max - min; }
    float Extent(size_t dim) const { return max[dim] - min[dim]; }

    Vector2 Center() const { return (min + max) * 0.5f; }
    float Center(size_t dim) const { return (min[dim] + max[dim]) * 0.5f; }

    bool Contains(const AABB& aabb) const {
        return
            min.x <= aabb.min.x &&
            aabb.max.x <= max.x &&
            min.y <= aabb.min.y &&
            aabb.max.y <= max.y;
    }
    bool Contains(const Vector2& point) const {
        return
            min.x <= point.x &&
            point.x <= max.x &&
            min.y <= point.y &&
            point.y <= max.y;
    }
    bool Intersects(const AABB& aabb) const {
        return
            min.x <= aabb.max.x &&
            aabb.min.x <= max.x &&
            min.y <= aabb.max.y &&
            aabb.min.y <= max.y;
    }
    size_t LongestAxis() const {
        return Extent(0) > Extent(1) ? 0 : 1;

    }

    Vector2 min, max;
};