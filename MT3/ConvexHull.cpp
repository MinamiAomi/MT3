#include "ConvexHull.h"

#include <stack>
#include <span>
#include <algorithm>

namespace {

    struct Segment {
        Vector2 start, end;

        Segment() : start({}), end({}) {}
        Segment(const Vector2& s, const Vector2& e) : start(s), end(e) {}
        explicit Segment(const Vector2& p) : start(p), end(p) {}

        float SignedDistance(const Vector2& p) { return Cross(end - start, p - start); }
    };

}

bool ConvexHull::Contains(const Vector2& point) const {
    for (size_t i = 0; i < vertices_.size(); ++i) {
        size_t j = (i + 1) % vertices_.size();
        float cross = Cross(vertices_[j] - vertices_[i], point - vertices_[i]);
        if (cross > 0.0f) {
            return false;
        }
    }
    return true;
}

Vector2 ConvexHull::Centroid() const {
    Vector2 centroid{};
    for (const auto& v : vertices_) {
        centroid += v;
    }
    return centroid * (1.0f / vertices_.size());
}

void ConvexHull::QuickHull(const std::vector<Vector2>& v) {
    vertices_.clear();
    std::vector<Vector2> vertices = v;

    // 分割した線分と線分の左側にある点
    std::stack<std::pair<Segment, std::span<Vector2>>> stack;

    // 分割線
    Segment splitSegment;
    // 左側を判定
    auto LeftSide = [&](const Vector2& v) {return splitSegment.SignedDistance(v) > 0.0f; };
    {
        // x軸の最小と最大の点を結んだ線分を最初の分割線とする
        auto iter = vertices.begin();
        splitSegment.start = splitSegment.end = *iter++;
        for (; iter != vertices.end(); ++iter) {
            if (iter->x < splitSegment.start.x) { splitSegment.start = *iter; }
            if (iter->x > splitSegment.end.x) { splitSegment.end = *iter; }
        }
        vertices_.emplace_back(splitSegment.start);
        vertices_.emplace_back(splitSegment.end);
        // 左側と右側を分ける
        auto split = std::partition(vertices.begin(), vertices.end(), LeftSide);
        // 線の始点から見て点の左側になるようにプッシュ
        stack.push({ splitSegment, std::span{vertices.begin(), split} });
        stack.push({ {splitSegment.end, splitSegment.start}, std::span{split, vertices.end()} });
    }

    while (!stack.empty()) {
        auto baseSegment = stack.top().first;
        auto range = stack.top().second;
        stack.pop();

        // 線との最遠点を探す
        float farthestDistance = 0;
        auto farthestPos = range.end();
        for (auto iter = range.begin(); iter != range.end(); ++iter) {
            float distance = baseSegment.SignedDistance(*iter);
            if (distance > farthestDistance) {
                farthestDistance = distance;
                farthestPos = iter;
            }
        }
        // 最遠点がない
        if (farthestPos == range.end()) { continue; }

        // 最遠点を追加
        vertices_.emplace_back(*farthestPos);
        // 始点と最遠点を結ぶ線で分ける
        splitSegment = { baseSegment.start, *farthestPos };
        auto split1 = std::partition(range.begin(), range.end(), LeftSide);
        // 点がある場合スタックにプッシュ
        if (split1 != range.begin()) {
            stack.push({ splitSegment, std::span{range.begin(), split1} });
        }
        // 最遠点と終点を結ぶ線で分ける
        splitSegment = { *farthestPos, baseSegment.end };
        auto split2 = std::partition(split1, range.end(), LeftSide);
        // 点がある場合スタックにプッシュ
        if (split2 != split1) {
            stack.push({ splitSegment, std::span{split1, split2 } });
        }
    }
    SortPointsClockwise();
}

ConvexHull ConvexHull::MinkowskiDifference(const ConvexHull& hull1, const Matrix4x4& worldMat1, const ConvexHull& hull2, const Matrix4x4& worldMat2) {
    std::vector<Vector2> vertices(hull1.GetVertices().size() * hull2.GetVertices().size());
    auto iter = vertices.begin();
    //Matrix4x4 mat = worldMat1 * worldMat2;
    for (const auto& v1 : hull1.GetVertices()) {
        Vector3 wv1 = ToVector3(v1) * worldMat1;
        for (const auto& v2 : hull2.GetVertices()) {
            Vector3 wv2 = ToVector3(v2) * worldMat2;
            *iter = ToVector2(wv1 - wv2);
           // *iter = ToVector2(ToVector3(v1 - v2) * Inverse(mat));
            ++iter;
        }
    }
    ConvexHull result;
    result.QuickHull(vertices);
    return result;
}

void ConvexHull::SortPointsClockwise() {
    Vector2 centroid = Centroid();

    auto LessCW = [&](const Vector2& v1, const Vector2& v2) {
        Vector2 ctov1 = v1 - centroid;
        Vector2 ctov2 = v2 - centroid;

        if (ctov1.x >= 0.0f && ctov2.x < 0.0f) { return true; }
        if (ctov1.x < 0.0f && ctov2.x >= 0) { return false; }

        if (ctov1.x == 0.0f && ctov2.x == 0.0f) {
            if (ctov1.y >= 0.0f && ctov2.y >= 0.0f) {
                return ctov1.y > ctov2.y;
            }
            return ctov1.y < ctov2.y;
        }

        float det = Cross(-ctov2, ctov1);
        if (det < 0.0f) { return true; }
        if (det > 0.0f) { return false; }

        return LengthSquare(ctov1) > LengthSquare(ctov2);
    };

    std::sort(vertices_.begin(), vertices_.end(), LessCW);
}
