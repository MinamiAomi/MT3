#pragma once

#include <vector>

#include "MyMath.h"

class ConvexHull {
public:
    ConvexHull() {}
    explicit ConvexHull(const std::vector<Vector2>& vertices) : vertices_(vertices) {}
    explicit ConvexHull(std::vector<Vector2>&& vertices) : vertices_(vertices) {}

    Vector2 Centroid() const;
    bool Contains(const Vector2& point) const;
    void QuickHull(const std::vector<Vector2>& vertices);

    static ConvexHull MinkowskiDifference(const ConvexHull& hull1, const Matrix4x4& worldMat1, const ConvexHull& hull2, const Matrix4x4& worldMat2);

    const std::vector<Vector2>& GetVertices() const { return vertices_; }

private:
    void SortPointsClockwise();

    std::vector<Vector2> vertices_;
};