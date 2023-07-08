#pragma once

#include <vector>

#include "MyMath.h"

class ConvexPolygon {
public:
    ConvexPolygon() {}
    explicit ConvexPolygon(const std::vector<Vector2>& vertices) : vertices(vertices) {}

    ConvexPolygon MinkowskiDifference(const ConvexPolygon& other) const {
        ConvexPolygon result;

        for (const auto& v1 : vertices) {
            for (const auto& v2 : other.vertices) {
                result.vertices.emplace_back(v1 - v2);
            }
        }

        return result;
    }

    std::vector<Vector2> vertices;
};