#pragma once
#include "Geometry.h"

bool IsCollision(const Sphere& sphere1, const Sphere& sphere2);
bool IsCollision(const Sphere& sphere, const Plane& plane);
bool IsCollision(const Line& line, const Plane& plane);
bool IsCollision(const Ray& ray, const Plane& plane);
bool IsCollision(const Segment& segment, const Plane& plane);
