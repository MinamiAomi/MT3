#pragma once
#include "Geometry.h"

bool IsCollision(const Sphere& sphere1, const Sphere& sphere2);
bool IsCollision(const Sphere& sphere, const Plane& plane);
bool IsCollision(const Plane& plane, const Line& line);
bool IsCollision(const Plane& plane, const Ray& ray);
bool IsCollision(const Plane& plane, const Segment& segment);
bool IsCollision(const Triangle& triangle, const Line& line);
bool IsCollision(const Triangle& triangle, const Ray& ray);
bool IsCollision(const Triangle& triangle, const Segment& segment);
bool IsCollision(const AABB& aabb1, const AABB& aabb2);
bool IsCollision(const AABB& aabb, const Sphere& sphere);
bool IsCollision(const AABB& aabb, const Line& line);
bool IsCollision(const AABB& aabb, const Ray& ray);
bool IsCollision(const AABB& aabb, const Segment& segment);
