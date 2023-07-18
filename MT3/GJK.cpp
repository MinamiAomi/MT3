#include "GJK.h"

#include <initializer_list>
#include <algorithm>  

namespace {

    Vector2 Support(const CollisionShape& a, const CollisionShape& b, const Vector2& direction) {
        return a.FindFurthestPoint(direction) - b.FindFurthestPoint(-direction);
    }

    bool LineCase(Simplex& simplex, Vector2& direction) {
        Vector2 a = simplex[0];
        Vector2 b = simplex[1];

        Vector2 ab = b - a;
        Vector2 ao = -a;

        direction = TripleProduct(ab, ao, ab);
        return false;
    }

    bool TriangleCase(Simplex& simplex, Vector2& direction) {
        Vector2 a = simplex[0];
        Vector2 b = simplex[1];
        Vector2 c = simplex[2];

        Vector2 ab = b - a;
        Vector2 ac = c - a;
        Vector2 ao = -a;

        Vector2 acPerp = TripleProduct(ab, ac, ac);
        float acaoDot = Dot(acPerp, ao);
        if (acaoDot > 0.0f) {
            simplex = { a, c };
            direction = acPerp;
            return false;
        }
        Vector2 abPerp = TripleProduct(ac, ab, ab);
        float abaoDot = Dot(abPerp, ao);
        if (abaoDot > 0.0f) {
            simplex = { a, b };
            direction = abPerp;
            return false;
        }
        // 原点がac上に存在
        if (acaoDot == 0.0f) { simplex = { a, c }; }
        // 原点がab上に存在
        if (abaoDot == 0.0f) { simplex = { a, b }; }
        return true;
    }

    bool NearestSimplex(Simplex& simplex, Vector2& direction) {
        switch (simplex.size()) {
        case 2: return LineCase(simplex, direction);
        case 3: return TriangleCase(simplex, direction);
        }
        return false;
    }
}

Simplex::Simplex() : points_({}), size_(0) {}
Simplex& Simplex::operator=(const std::initializer_list<Vector2>& list) {
    for (auto iter = list.begin(); iter != list.end(); ++iter) {
        points_[std::distance(list.begin(), iter)] = *iter;
    }
    size_ = uint32_t(list.size());
    return *this;
}

void Simplex::push_front(const Vector2& point) {
    points_ = { point, points_[0], points_[1] };
    size_ = std::min(size_ + 1, 3u);
}

bool NarrowPhaseAABB(const CollisionShape& collisionShape1, const CollisionShape& collisionShape2) {
    return
        (collisionShape1.GetAABB().min.x <= collisionShape2.GetAABB().max.x && collisionShape1.GetAABB().max.x >= collisionShape2.GetAABB().min.x) &&
        (collisionShape1.GetAABB().min.y <= collisionShape2.GetAABB().max.y && collisionShape1.GetAABB().max.y >= collisionShape2.GetAABB().min.y);
}

bool GJK(const CollisionShape& collisionShape1, const CollisionShape& collisionShape2, Simplex* simplex) {
    // 初めはX座標の最大を取得
    Vector2 direction = Vector2UnitX;
    Vector2 support = Support(collisionShape1, collisionShape2, direction);

    Simplex points;
    points.push_front(support);

    direction = -support;

    while (true) {
        support = Support(collisionShape1, collisionShape2, direction);
        if (Dot(support, direction) <= 0.0f) { return false; }
        points.push_front(support);

        if (NearestSimplex(points, direction)) {
            if (simplex) { *simplex = points; }
            return true;
        }
    }
    return false;
}

Vector2 EPA(const Simplex& simplex, const CollisionShape& collisionShape1, const CollisionShape& collisionShape2) {
    // 単体を構成する頂点が足りていない
    if (simplex.size() < 3) { return {}; }

    std::vector<Vector2> polygon(simplex.begin(), simplex.end());

    struct Edge {
        size_t startIndex;
        size_t endIndex;
        Vector2 normal;
        float distance;

        Edge(const std::vector<Vector2>& p, size_t si, size_t ei) :
            startIndex(si),
            endIndex(ei) {
            normal = Normalize(Perpendicular(p[ei] - p[si]));
            distance = Dot(p[si], normal);
            // 法線を外側に向ける
            if (distance < 0.0f) {
                distance = -distance;
                normal = -normal;
            }
        }
    };

    std::vector<Edge> edges{
        Edge{ polygon, 0, 1 },
        Edge{ polygon, 1, 2 },
        Edge{ polygon, 2, 0 } };
    auto closestEdge = edges.begin();

    const size_t kMaxLoopCount = 50;
    size_t loopCount = 0;

    while (true) {
        for (auto iter = (edges.begin() + 1); iter != edges.end(); ++iter) {
            if (iter->distance < closestEdge->distance) {
                closestEdge = iter;
            }
        }

        Vector2 support = Support(collisionShape1, collisionShape2, closestEdge->normal);
        float sd = Dot(support, closestEdge->normal);
        if (std::abs(sd - closestEdge->distance) > 0.01f && loopCount++ < kMaxLoopCount) {
            polygon.emplace_back(support);
            size_t s = closestEdge->startIndex;
            size_t e = closestEdge->endIndex;
            std::swap(*closestEdge, edges.back());
            edges.pop_back();
            edges.emplace_back(polygon, s, polygon.size() - 1);
            edges.emplace_back(polygon, polygon.size() - 1, e);
            closestEdge = edges.begin();
        }
        else {
            break;
        }
    }
    return closestEdge->normal * (closestEdge->distance + 0.01f);
}

bool CollisionTerrain(const std::vector<Vector2>& terrain, const Vector2& up, const CollisionShape& collisionShape, std::vector<size_t>& index) {

    for (size_t i = 0; i < terrain.size() - 1; ++i) {
        Vector2 s = terrain[i];
        Vector2 e = terrain[i + 1];

        Vector2 se = e - s;
        Vector2 es = s - e;

        Vector2 n = Perpendicular(se);
        Vector2 p0 = collisionShape.FindFurthestPoint(n) - s;
        Vector2 p1 = collisionShape.FindFurthestPoint(-n) - s;
        Vector2 p2 = collisionShape.FindFurthestPoint(se) - s;
        Vector2 p3 = collisionShape.FindFurthestPoint(es) - e;

        float dot = Dot(p0, p1);

        if (dot <= 0.0f) {
            index.emplace_back(i);
        }


    }
    up;
    return false;
}
