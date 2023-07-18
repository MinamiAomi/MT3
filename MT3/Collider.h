#pragma once

#include "MyMath.h"

class Collider {
public:
    ~Collider() {}
    virtual Vector3 FindFurthestPoint(const Vector3& direction) const = 0;    
};