#pragma once

#include <vector>

#include "MyMath.h"
#include "Collider.h"

struct GJKResult {
    std::vector<Vector3> simplex;
};

bool GJK(const Collider& collider1, const Collider& collider2, GJKResult* gjkResult);