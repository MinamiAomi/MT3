#include "Collision.h"
#include "MyMath.h"

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
