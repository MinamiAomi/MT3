#include "Collision.h"
#include "MyMath.h"

bool IsCollision(const Sphere& s1, const Sphere& s2) {
    return LengthSquare(s2.center - s1.center) <= (s1.radius + s2.radius) * (s1.radius + s2.radius);
}
