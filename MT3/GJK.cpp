#include "GJK.h"

#include <initializer_list>
#include <algorithm>  

namespace {

    Vector2 Support(const CollisionShape& a, const CollisionShape& b, const Vector2& direction, std::vector<std::pair<Vector2, Vector2>>& p1) {
        p1.emplace_back(a.FindFurthestPoint(direction), b.FindFurthestPoint(-direction));
        return a.FindFurthestPoint(direction) - b.FindFurthestPoint(-direction);
    }

    bool SameDirection(const Vector2& direction, const Vector2& ao) {
        return Dot(direction, ao) > 0.0f;
    }

    bool Line(Simplex& simplex, Vector2& direction) {
        Vector2 a = simplex[0];
        Vector2 b = simplex[1];

        Vector2 ab = b - a;
        Vector2 ao = -a;

        if (Dot(ab, ao) > 0.0f) {
            direction = Perpendicular(ab);
            if (Dot(direction, ao) < 0.0f) { direction = -direction; }
            return false;
        }
        simplex = { a };
        direction = ao;
        return false;
    }

    bool Triangle(Simplex& simplex, Vector2& direction) {
        Vector2 a = simplex[0];
        Vector2 b = simplex[1];
        Vector2 c = simplex[2];

        Vector2 ab = b - a;
        Vector2 ac = c - a;
        Vector2 ao = -a;

        Vector2 abPrep = Perpendicular(ab);
        if (Dot(abPrep, ac) > 0.0f) { abPrep = -abPrep; }
        Vector2 acPrep = Perpendicular(ac);
        if (Dot(acPrep, ab) > 0.0f) { acPrep = -acPrep; }

        if (Dot(abPrep, ao) > 0.0f) {
            simplex = { a, b };
            direction = abPrep;
            return false;
        }
        if (Dot(acPrep, ao) > 0.0f) {
            simplex = { a, c };
            direction = acPrep;
            return false;
        }
       
        return true;
    }

    bool NearestSimplex(Simplex& simplex, Vector2& direction) {
        switch (simplex.size()) {
        case 2: return Line(simplex, direction);
        case 3: return Triangle(simplex, direction);
        }
        return false;
    }
}

bool GJK(const CollisionShape& collisionShape1, const CollisionShape& collisionShape2, std::vector<std::pair<Vector2, Vector2>>& p, std::vector<std::pair<Vector2, Vector2>>& p1) {
    p.clear();
    // 初めはX座標の最大を取得
    Vector2 direction = Vector2UnitX;
    Vector2 support = Support(collisionShape1, collisionShape2, direction, p1);
    Simplex simplex;
    simplex.push_front(support);
    p.emplace_back(support, direction);

    direction = -support;

    while (true) {
        support = Support(collisionShape1, collisionShape2, direction, p1);
        if (Dot(support, direction) < 0.0f) { return false; }
        simplex.push_front(support);
        p.emplace_back(support, direction);

        if (NearestSimplex(simplex, direction)) {
            return true;
        }
    }
    return false;
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



