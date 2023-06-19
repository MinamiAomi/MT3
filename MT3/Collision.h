#pragma once
#include "Geometry.h"

namespace Collision {
    bool IsCollision(const Geometry::Sphere& sphere1, const Geometry::Sphere& sphere2);
    bool IsCollision(const Geometry::Sphere& sphere, const Geometry::Plane& plane);
    bool IsCollision(const Geometry::Plane& plane, const Geometry::Line& line);
    bool IsCollision(const Geometry::Plane& plane, const Geometry::Ray& ray);
    bool IsCollision(const Geometry::Plane& plane, const Geometry::Segment& segment);
    bool IsCollision(const Geometry::Triangle& triangle, const Geometry::Line& line);
    bool IsCollision(const Geometry::Triangle& triangle, const Geometry::Ray& ray);
    bool IsCollision(const Geometry::Triangle& triangle, const Geometry::Segment& segment);
    bool IsCollision(const Geometry::AABB& aabb1, const Geometry::AABB& aabb2);
    bool IsCollision(const Geometry::AABB& aabb, const Geometry::Sphere& sphere);
    bool IsCollision(const Geometry::AABB& aabb, const Geometry::Line& line);
    bool IsCollision(const Geometry::AABB& aabb, const Geometry::Ray& ray);
    bool IsCollision(const Geometry::AABB& aabb, const Geometry::Segment& segment);
    bool IsCollision(const Geometry::OBB& obb, const Geometry::Sphere& sphere);
    bool IsCollision(const Geometry::OBB& obb, const Geometry::Line& line);
    bool IsCollision(const Geometry::OBB& obb, const Geometry::Ray& ray);
    bool IsCollision(const Geometry::OBB& obb, const Geometry::Segment& segment);
    bool IsCollision(const Geometry::OBB& obb, const Geometry::AABB& aabb);
    bool IsCollision(const Geometry::OBB& obb1, const Geometry::OBB& obb2);
} // namespace