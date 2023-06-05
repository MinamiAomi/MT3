#pragma once
#include <Vector3.h>


// 球
struct Sphere {
	Vector3 center;	// 中心
	float radius;	// 半径
};
// 直線
struct Line {
	Vector3 origin; // 始点
	Vector3 diff;	// 終点への差分ベクトル
};
// 半直線
struct Ray {
	Vector3 origin; // 始点
	Vector3 diff;	// 終点への差分ベクトル
};
// 線分
struct Segment {
	Vector3 origin; // 始点
	Vector3 diff;	// 終点への差分ベクトル
};
// 平面
struct Plane {
	Vector3 normal; // 法線
	float distance; // 距離
};
// 三角形
struct Triangle {
	Vector3 vertices[3]; // 頂点
};
// AABB
struct AABB {
	Vector3 min; // 最小点
	Vector3 max; // 最大点
};
struct OBB {
	Vector3 center;				// 中心
	Vector3 orientations[3];	// 姿勢
	Vector3 size;				// 大きさ
};

Plane MakePlaneFromPointAndNormal(const Vector3& point, const Vector3 normal);
Plane MakePlaneFromTriangle(const Triangle& triangle);

Vector3 ClosestPoint(const Vector3& point, const Line& line);
Vector3 ClosestPoint(const Vector3& point, const Ray& ray);
Vector3 ClosestPoint(const Vector3& point, const Segment& segment);
Vector3 ClosestPoint(const AABB& aabb, const Sphere& sphere);
Vector3 ClosestPoint(const OBB& obb, const Sphere& sphere);

bool Intersection(const Plane& plane, const Line& line, Vector3& out_intersectionPoint);
bool Intersection(const Plane& plane, const Ray& ray, Vector3& out_intersectionPoint);
bool Intersection(const Plane& plane, const Segment& segment, Vector3& out_intersectionPoint);
bool Intersection(const Triangle& triangle, const Line& line, Vector3& out_intersectionPoint);
bool Intersection(const Triangle& triangle, const Ray& ray, Vector3& out_intersectionPoint);
bool Intersection(const Triangle& triangle, const Segment& segment, Vector3& out_intersectionPoint);

Vector3 CalcNormal(const Vector3& a, const Vector3& b, const Vector3& c);
Vector3 CalcNormal(const Triangle& triangle);
