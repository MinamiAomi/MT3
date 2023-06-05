#include "Collision.h"
#include "MyMath.h"
#include <algorithm>

bool IsCollision(const Sphere& s1, const Sphere& s2) {
	return LengthSquare(s2.center - s1.center) <= (s1.radius + s2.radius) * (s1.radius + s2.radius);
}

bool IsCollision(const Sphere& sphere, const Plane& plane) {
	float dis = Dot(plane.normal, sphere.center) - plane.distance;
	return std::abs(dis) <= sphere.radius;
}

bool IsCollision(const Plane& plane, const Line& line) {
	Vector3 dummy{};
	return Intersection(plane, line, dummy);
}

bool IsCollision(const Plane& plane, const Ray& ray) {
	Vector3 dummy{};
	return Intersection(plane, ray, dummy);
}

bool IsCollision(const Plane& plane, const Segment& segment) {
	Vector3 dummy{};
	return Intersection(plane, segment, dummy);
}

bool IsCollision(const Triangle& triangle, const Line& line) {
	Vector3 dummy{};
	return Intersection(triangle, line, dummy);
}

bool IsCollision(const Triangle& triangle, const Ray& ray) {
	Vector3 dummy{};
	return Intersection(triangle, ray, dummy);
}

bool IsCollision(const Triangle& triangle, const Segment& segment) {
	Vector3 dummy{};
	return Intersection(triangle, segment, dummy);
}

bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	return
		(aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
		(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z);
}

bool IsCollision(const AABB& aabb, const Sphere& sphere) {
	float disSqu = LengthSquare(ClosestPoint(aabb, sphere) - sphere.center);
	return disSqu <= sphere.radius * sphere.radius;
}

bool IsCollision(const AABB& aabb, const Line& line) {
	float tXMin = (aabb.min.x - line.origin.x) / line.diff.x;
	float tXMax = (aabb.max.x - line.origin.x) / line.diff.x;
	float tYMin = (aabb.min.y - line.origin.y) / line.diff.y;
	float tYMax = (aabb.max.y - line.origin.y) / line.diff.y;
	float tZMin = (aabb.min.z - line.origin.z) / line.diff.z;
	float tZMax = (aabb.max.z - line.origin.z) / line.diff.z;

	// 軸に平行かついずれかがmin/maxと一致
	if (std::isnan(tXMin) || std::isnan(tXMax) ||
		std::isnan(tYMin) || std::isnan(tYMax) ||
		std::isnan(tZMin) || std::isnan(tZMax)) {
		return true;
	}

	float tNearX = (std::min)(tXMin, tXMax);
	float tFarX = (std::max)(tXMin, tXMax);
	float tNearY = (std::min)(tYMin, tYMax);
	float tFarY = (std::max)(tYMin, tYMax);
	float tNearZ = (std::min)(tZMin, tZMax);
	float tFarZ = (std::max)(tZMin, tZMax);

	float tMin = (std::max)((std::max)(tNearX, tNearY), tNearZ);
	float tMax = (std::min)((std::min)(tFarX, tFarY), tFarZ);

	if (tMin > tMax) { return false; }
	return true;
}

bool IsCollision(const AABB& aabb, const Ray& ray) {
	float tXMin = (aabb.min.x - ray.origin.x) / ray.diff.x;
	float tXMax = (aabb.max.x - ray.origin.x) / ray.diff.x;
	float tYMin = (aabb.min.y - ray.origin.y) / ray.diff.y;
	float tYMax = (aabb.max.y - ray.origin.y) / ray.diff.y;
	float tZMin = (aabb.min.z - ray.origin.z) / ray.diff.z;
	float tZMax = (aabb.max.z - ray.origin.z) / ray.diff.z;

	// 軸に平行かついずれかがmin/maxと一致
	if (std::isnan(tXMin) || std::isnan(tXMax) ||
		std::isnan(tYMin) || std::isnan(tYMax) ||
		std::isnan(tZMin) || std::isnan(tZMax)) {
		return true;
	}

	float tNearX = (std::min)(tXMin, tXMax);
	float tFarX = (std::max)(tXMin, tXMax);
	float tNearY = (std::min)(tYMin, tYMax);
	float tFarY = (std::max)(tYMin, tYMax);
	float tNearZ = (std::min)(tZMin, tZMax);
	float tFarZ = (std::max)(tZMin, tZMax);

	float tMin = (std::max)((std::max)(tNearX, tNearY), tNearZ);
	float tMax = (std::min)((std::min)(tFarX, tFarY), tFarZ);

	if (tMin > tMax) { return false; }
	// 始点側は判定無し
	if (tMin < 0.0f && tMax < 0.0f) { return false; }

	return true;
}

bool IsCollision(const AABB& aabb, const Segment& segment) {
	float tXMin = (aabb.min.x - segment.origin.x) / segment.diff.x;
	float tXMax = (aabb.max.x - segment.origin.x) / segment.diff.x;
	float tYMin = (aabb.min.y - segment.origin.y) / segment.diff.y;
	float tYMax = (aabb.max.y - segment.origin.y) / segment.diff.y;
	float tZMin = (aabb.min.z - segment.origin.z) / segment.diff.z;
	float tZMax = (aabb.max.z - segment.origin.z) / segment.diff.z;

	// 軸に平行かついずれかがmin/maxと一致
	if (std::isnan(tXMin) || std::isnan(tXMax) ||
		std::isnan(tYMin) || std::isnan(tYMax) ||
		std::isnan(tZMin) || std::isnan(tZMax)) {
		return true;
	}

	float tNearX = (std::min)(tXMin, tXMax);
	float tFarX = (std::max)(tXMin, tXMax);
	float tNearY = (std::min)(tYMin, tYMax);
	float tFarY = (std::max)(tYMin, tYMax);
	float tNearZ = (std::min)(tZMin, tZMax);
	float tFarZ = (std::max)(tZMin, tZMax);

	float tMin = (std::max)((std::max)(tNearX, tNearY), tNearZ);
	float tMax = (std::min)((std::min)(tFarX, tFarY), tFarZ);

	if (tMin > tMax) { return false; }
	// 線分は制限あり
	if (tMin < 0.0f && tMax < 0.0f || tMin > 1.0f && tMax > 1.0f) { return false; }

	return true;
}

bool IsCollision(const OBB& obb, const Sphere& sphere) {
	Matrix4x4 obbWorldInverse = MakeInverseMatrix(MakeRotateMatrixFromOrientations(obb.orientations), obb.center);
	Vector3 centerInOBBLocalSpace = sphere.center * obbWorldInverse;
	AABB aabbOBBLocal{ .min = -obb.size, .max = obb.size };
	Sphere sphereObbLocal{ centerInOBBLocalSpace, sphere.radius };
	return IsCollision(aabbOBBLocal, sphereObbLocal);
}
