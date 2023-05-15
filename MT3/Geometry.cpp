#include "Geometry.h"
#include "MyMath.h"
#include <algorithm>

Plane MakePlaneFromPointAndNormal(const Vector3& point, const Vector3 normal) {
	return { normal, Dot(point,normal) };
}

Plane MakePlaneFromTriangle(const Triangle& triangle) {
	Vector3 normal = CalcNormal(triangle);
	return MakePlaneFromPointAndNormal(triangle.vertices[0], normal);
}

Vector3 ClosestPoint(const Vector3& point, const Line& line) {
	return line.origin + Project(point - line.origin, line.diff);
}

Vector3 ClosestPoint(const Vector3& point, const Ray& ray) {
	Vector3 rayDir = Normalize(ray.diff);
	float dis = Dot(point - ray.origin, rayDir);
	dis = (std::max)(dis, 0.0f);
	Vector3 proj = dis * rayDir;
	return ray.origin + proj;
}

Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	float segLen = Length(segment.diff);
	Vector3 segDir = segment.diff * (1.0f / segLen);
	float dis = Dot(point - segment.origin, segDir);
	dis = std::clamp(dis, 0.0f, segLen);
	Vector3 proj = dis * segDir;
	return segment.origin + proj;
}

Vector3 ClosestPoint(const AABB& aabb, const Sphere& sphere) {
	return {
		std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
		std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
		std::clamp(sphere.center.z, aabb.min.z, aabb.max.z), };
}

bool Intersection(const Plane& plane, const Line& line, Vector3& out_intersectionPoint) {
	float DdotN = Dot(line.diff, plane.normal);
	if (DdotN == 0.0f) { return false; }
	float t = (plane.distance - Dot(line.origin, plane.normal)) / DdotN;
	out_intersectionPoint = line.origin + t * line.diff;
	return true;
}

bool Intersection(const Plane& plane, const Ray& ray, Vector3& out_intersectionPoint) {
	float DdotN = Dot(ray.diff, plane.normal);
	if (DdotN == 0.0f) { return false; }
	float t = (plane.distance - Dot(ray.origin, plane.normal)) / DdotN;
	if (0.0f > t) { return false; }
	out_intersectionPoint = ray.origin + t * ray.diff;
	return true;
}

bool Intersection(const Plane& plane, const Segment& segment, Vector3& out_intersectionPoint) {
	float DdotN = Dot(segment.diff, plane.normal);
	if (DdotN == 0.0f) { return false; }
	float t = (plane.distance - Dot(segment.origin, plane.normal)) / DdotN;
	if (0.0f > t || t > 1.0f) { return false; }
	out_intersectionPoint = segment.origin + t * segment.diff;
	return true;
}

bool Intersection(const Triangle& triangle, const Line& line, Vector3& out_intersectionPoint) {
	// 三角形から平面を求める
	Vector3 normal = CalcNormal(triangle);
	Plane plane = MakePlaneFromPointAndNormal(triangle.vertices[0], normal);
	Vector3 point{};
	// 平面と線の交点を求める
	if (!Intersection(plane, line, point)) { return false; }
	// 三角形の２つの頂点と交点から生成される三角形の法線を求める
	Vector3 normal01 = CalcNormal(triangle.vertices[0], triangle.vertices[1], point);
	Vector3 normal12 = CalcNormal(triangle.vertices[1], triangle.vertices[2], point);
	Vector3 normal20 = CalcNormal(triangle.vertices[2], triangle.vertices[0], point);
	// すべての法線が同じ方向を向いていたら衝突
	if (!(Dot(normal01, normal) >= 0.0f && 
		  Dot(normal12, normal) >= 0.0f && 
		  Dot(normal20, normal) >= 0.0f)) {
		return false;
	}
	out_intersectionPoint = point;
	return true;
}

bool Intersection(const Triangle& triangle, const Ray& ray, Vector3& out_intersectionPoint) {
	// 三角形から平面を求める
	Vector3 normal = CalcNormal(triangle);
	Plane plane = MakePlaneFromPointAndNormal(triangle.vertices[0], normal);
	Vector3 point{};
	// 平面と線の交点を求める
	if (!Intersection(plane, ray, point)) { return false; }
	// 三角形の２つの頂点と交点から生成される三角形の法線を求める
	Vector3 normal01 = CalcNormal(triangle.vertices[0], triangle.vertices[1], point);
	Vector3 normal12 = CalcNormal(triangle.vertices[1], triangle.vertices[2], point);
	Vector3 normal20 = CalcNormal(triangle.vertices[2], triangle.vertices[0], point);
	// すべての法線が同じ方向を向いていたら衝突
	if (!(Dot(normal01, normal) >= 0.0f && 
		  Dot(normal12, normal) >= 0.0f && 
		  Dot(normal20, normal) >= 0.0f)) {
		return false;
	}
	out_intersectionPoint = point;
	return true;
}

bool Intersection(const Triangle& triangle, const Segment& segment, Vector3& out_intersectionPoint) {
	// 三角形から平面を求める
	Vector3 normal = CalcNormal(triangle);
	Plane plane = MakePlaneFromPointAndNormal(triangle.vertices[0], normal);
	Vector3 point{};
	// 平面と線の交点を求める
	if (!Intersection(plane, segment, point)) { return false; }
	// 三角形の２つの頂点と交点から生成される三角形の法線を求める
	Vector3 normal01 = CalcNormal(triangle.vertices[0], triangle.vertices[1], point);
	Vector3 normal12 = CalcNormal(triangle.vertices[1], triangle.vertices[2], point);
	Vector3 normal20 = CalcNormal(triangle.vertices[2], triangle.vertices[0], point);
	// すべての法線が同じ方向を向いていたら衝突
	if (!(Dot(normal01, normal) >= 0.0f && 
		  Dot(normal12, normal) >= 0.0f && 
		  Dot(normal20, normal) >= 0.0f)) {
		return false;
	}
	out_intersectionPoint = point;
	return true;
}

Vector3 CalcNormal(const Vector3& a, const Vector3& b, const Vector3& c) {
	return Cross(b - a, c - b);
}

Vector3 CalcNormal(const Triangle& triangle) {
	return Cross(triangle.vertices[1] - triangle.vertices[0], triangle.vertices[2] - triangle.vertices[1]);
}
