#include "Collision.h"
#include "MyMath.h"

bool IsCollision(const Sphere& s1, const Sphere& s2) {
    return LengthSquare(s2.center - s1.center) <= (s1.radius + s2.radius) * (s1.radius + s2.radius);
}

bool IsCollision(const Sphere& sphere, const Plane& plane) {
    float dis = Dot(plane.normal, sphere.center) - plane.distance;
    return std::abs(dis) <= sphere.radius;
}

bool IsCollision(const Line& line, const Plane& plane) {
    float DdotN = Dot(line.diff, plane.normal);
    if (DdotN == 0.0f) {
        return false;
    }
    return true;
}

bool IsCollision(const Ray& ray, const Plane& plane) {
    float DdotN = Dot(ray.diff, plane.normal);
    if (DdotN == 0.0f) {
        return false;
    }
    float t = (plane.distance - Dot(ray.origin, plane.normal)) / DdotN;
    return 0.0f <= t;
}

bool IsCollision(const Segment& segment, const Plane& plane) {
    float DdotN = Dot(segment.diff, plane.normal);
    if (DdotN == 0.0f) {
        return false;
    }
    float t = (plane.distance - Dot(segment.origin, plane.normal)) / DdotN;
    return 0.0f <= t && t <= 1.0f;
}
