#include "Collision.h"
#include "MyMath.h"

bool IsCollision(const Sphere& s1, const Sphere& s2) {
    return LengthSquare(s2.center - s1.center) <= (s1.radius + s2.radius) * (s1.radius + s2.radius);
}

bool IsCollision(const Sphere& sphere, const Plane& plane) {
    float dis = Dot(plane.normal, sphere.center) - plane.distance;
    return std::abs(dis) <= sphere.radius;
}
