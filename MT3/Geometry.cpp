#include "Geometry.h"
#include "MyMath.h"
#include <algorithm>
#include <cassert>

namespace Geometry {

    Plane MakePlaneFromPointAndNormal(const Vector3& point, const Vector3 normal) {
        return { normal, Dot(point,normal) };
    }

    Plane MakePlaneFromTriangle(const Triangle& triangle) {
        Vector3 normal = CalcNormal(triangle);
        return MakePlaneFromPointAndNormal(triangle.vertices[0], normal);
    }

    float SignedDistanceToPlane(const Vector3& point, const Plane& plane) {
        return Dot(plane.normal, point) - plane.distance;
    }

    Vector3 ClosestPoint(const Vector3& point, const Line& line) {
        return line.origin + Project(point - line.origin, line.diff);
    }

    Vector3 ClosestPoint(const Vector3& point, const Ray& ray) {
        Vector3 rayDir = Normalize(ray.diff);
        float dis = Dot(point - ray.origin, rayDir);
        dis = (std::max)(dis, 0.0f);
        Vector3 proj = dis * rayDir;
        return ray.origin + proj;
    }

    Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
        float segLen = Length(segment.diff);
        Vector3 segDir = segment.diff * (1.0f / segLen);
        float dis = Dot(point - segment.origin, segDir);
        dis = std::clamp(dis, 0.0f, segLen);
        Vector3 proj = dis * segDir;
        return segment.origin + proj;
    }

    Vector3 ClosestPoint(const Vector3& point, const Plane& plane) {
        float d = Dot(plane.normal, point) - plane.distance;
        return point - d * plane.normal;
    }

    Vector3 ClosestPoint(const AABB& aabb, const Sphere& sphere) {
        return {
            std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
            std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
            std::clamp(sphere.center.z, aabb.min.z, aabb.max.z), };
    }

    Vector3 ClosestPoint(const OBB& obb, const Sphere& sphere) {
        Matrix4x4 obbRotateMatrix = MakeRotateMatrixFromOrientations(obb.orientations);
        Matrix4x4 obbWorld = obbRotateMatrix;
        SetTranslate(obbWorld, obb.center);
        Matrix4x4 obbWorldInverse = MakeInverseMatrix(obbRotateMatrix, obb.center);

        Vector3 centerInOBBLocalSpace = sphere.center * obbWorldInverse;
        AABB aabbOBBLocal{ .min = -obb.size, .max = obb.size };
        Sphere sphereObbLocal{ centerInOBBLocalSpace, sphere.radius };

        return ClosestPoint(aabbOBBLocal, sphereObbLocal) * obbWorld;
    }

    bool Intersection(const Plane& plane, const Line& line, Vector3& out_intersectionPoint) {
        float DdotN = Dot(line.diff, plane.normal);
        if (DdotN == 0.0f) { return false; }
        float t = (plane.distance - Dot(line.origin, plane.normal)) / DdotN;
        out_intersectionPoint = line.origin + t * line.diff;
        return true;
    }

    bool Intersection(const Plane& plane, const Ray& ray, Vector3& out_intersectionPoint) {
        float DdotN = Dot(ray.diff, plane.normal);
        if (DdotN == 0.0f) { return false; }
        float t = (plane.distance - Dot(ray.origin, plane.normal)) / DdotN;
        if (0.0f > t) { return false; }
        out_intersectionPoint = ray.origin + t * ray.diff;
        return true;
    }

    bool Intersection(const Plane& plane, const Segment& segment, Vector3& out_intersectionPoint) {
        float DdotN = Dot(segment.diff, plane.normal);
        if (DdotN == 0.0f) { return false; }
        float t = (plane.distance - Dot(segment.origin, plane.normal)) / DdotN;
        if (0.0f > t || t > 1.0f) { return false; }
        out_intersectionPoint = segment.origin + t * segment.diff;
        return true;
    }

    bool Intersection(const Triangle& triangle, const Line& line, Vector3& out_intersectionPoint) {
        // 三角形から平面を求める
        Vector3 normal = CalcNormal(triangle);
        Plane plane = MakePlaneFromPointAndNormal(triangle.vertices[0], normal);
        Vector3 point{};
        // 平面と線の交点を求める
        if (!Intersection(plane, line, point)) { return false; }
        // 三角形の２つの頂点と交点から生成される三角形の法線を求める
        Vector3 normal01 = CalcNormal(triangle.vertices[0], triangle.vertices[1], point);
        Vector3 normal12 = CalcNormal(triangle.vertices[1], triangle.vertices[2], point);
        Vector3 normal20 = CalcNormal(triangle.vertices[2], triangle.vertices[0], point);
        // すべての法線が同じ方向を向いていたら衝突
        if (!(Dot(normal01, normal) >= 0.0f &&
            Dot(normal12, normal) >= 0.0f &&
            Dot(normal20, normal) >= 0.0f)) {
            return false;
        }
        out_intersectionPoint = point;
        return true;
    }

    bool Intersection(const Triangle& triangle, const Ray& ray, Vector3& out_intersectionPoint) {
        // 三角形から平面を求める
        Vector3 normal = CalcNormal(triangle);
        Plane plane = MakePlaneFromPointAndNormal(triangle.vertices[0], normal);
        Vector3 point{};
        // 平面と線の交点を求める
        if (!Intersection(plane, ray, point)) { return false; }
        // 三角形の２つの頂点と交点から生成される三角形の法線を求める
        Vector3 normal01 = CalcNormal(triangle.vertices[0], triangle.vertices[1], point);
        Vector3 normal12 = CalcNormal(triangle.vertices[1], triangle.vertices[2], point);
        Vector3 normal20 = CalcNormal(triangle.vertices[2], triangle.vertices[0], point);
        // すべての法線が同じ方向を向いていたら衝突
        if (!(Dot(normal01, normal) >= 0.0f &&
            Dot(normal12, normal) >= 0.0f &&
            Dot(normal20, normal) >= 0.0f)) {
            return false;
        }
        out_intersectionPoint = point;
        return true;
    }

    bool Intersection(const Triangle& triangle, const Segment& segment, Vector3& out_intersectionPoint) {
        // 三角形から平面を求める
        Vector3 normal = CalcNormal(triangle);
        Plane plane = MakePlaneFromPointAndNormal(triangle.vertices[0], normal);
        Vector3 point{};
        // 平面と線の交点を求める
        if (!Intersection(plane, segment, point)) { return false; }
        // 三角形の２つの頂点と交点から生成される三角形の法線を求める
        Vector3 normal01 = CalcNormal(triangle.vertices[0], triangle.vertices[1], point);
        Vector3 normal12 = CalcNormal(triangle.vertices[1], triangle.vertices[2], point);
        Vector3 normal20 = CalcNormal(triangle.vertices[2], triangle.vertices[0], point);
        // すべての法線が同じ方向を向いていたら衝突
        if (!(Dot(normal01, normal) >= 0.0f &&
            Dot(normal12, normal) >= 0.0f &&
            Dot(normal20, normal) >= 0.0f)) {
            return false;
        }
        out_intersectionPoint = point;
        return true;
    }

    Vector3 CalcNormal(const Vector3& a, const Vector3& b, const Vector3& c) {
        return Cross(b - a, c - b);
    }

    Vector3 CalcNormal(const Triangle& triangle) {
        return Cross(triangle.vertices[1] - triangle.vertices[0], triangle.vertices[2] - triangle.vertices[1]);
    }

    void GetVertices(const Vector3& min, const Vector3& max, std::vector<Vector3>& out_vertices) {
        out_vertices.clear();
        out_vertices = { {
            { min.x, min.y, min.z },   // 左下前
            { min.x, max.y, min.z },   // 左上前
            { max.x, max.y, min.z },   // 右上前
            { max.x, min.y, min.z },   // 右下前
            { min.x, min.y, max.z },   // 左下奥 
            { min.x, max.y, max.z },   // 左上奥
            { max.x, max.y, max.z },   // 右上奥
            { max.x, min.y, max.z },   // 右下奥
        } };
    }

    void GetVertices(const OBB& obb, std::vector<Vector3>& out_vertices) {
        GetVertices(-obb.size, obb.size, out_vertices);
        Matrix4x4 worldMatrix = MakeRotateMatrixFromOrientations(obb.orientations);
        SetTranslate(worldMatrix, obb.center);
        for (auto& vertex : out_vertices) {
            vertex = vertex * worldMatrix;
        }
    }

    bool IsSeparateAxis(const Vector3& normal, const std::vector<Vector3>& vertices1, const std::vector<Vector3>& vertices2) {
        assert(!vertices1.empty() && !vertices2.empty());
        float min1 = 0, max1 = 0;
        float min2 = 0, max2 = 0;
        min1 = max1 = Dot(normal, vertices1[0]);
        min2 = max2 = Dot(normal, vertices2[0]);
        for (size_t i = 1; i < vertices1.size(); ++i) {
            float dot1 = Dot(normal, vertices1[i]);
            min1 = (std::min)(dot1, min1);
            max1 = (std::max)(dot1, max1);
        }
        for (size_t i = 1; i < vertices2.size(); ++i) {
            float dot2 = Dot(normal, vertices2[i]);
            min2 = (std::min)(dot2, min2);
            max2 = (std::max)(dot2, max2);
        }
        float sumSpan = max1 - min1 + max2 - min2;
        float longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);
        return sumSpan < longSpan;
    }

}
