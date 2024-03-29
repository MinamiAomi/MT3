#pragma once
#include "MyMath.h"
#include "Geometry.h"
#include <vector>

enum class CullMode {
    kNone,
    kBack,
    kFront
};

class RenderingPipeline {
public:
    Vector3 cameraPosition{};
    Vector3 cameraRotate{};
    float targetDistance{};

    Matrix4x4 viewMatrix;
    Matrix4x4 projectionMatrix;
    Matrix4x4 viewportMatrix;

    void Initalize(float windowWidth, float windowHeight);
    void UpdateMatrix();

    const Matrix4x4& GetVPVMatrix()const { return vpvMatrix_; }
    const Matrix4x4& GetVPVMatrixInverse()const { return vpvMatrixInv_; }

    inline Vector3 Apply(const Vector3& v) {
        return Transform(v, vpvMatrix_);
    }

    void ScreenDrawLine(const Vector3& v1, const Vector3& v2, uint32_t color);
    void DrawLine(const Vector3& v1, const Vector3& v2, uint32_t color);
    void DrawGrid(float width = 10.0f, uint32_t subdivision = 10);
    void DrawBox(const Matrix4x4& world, uint32_t color);
    void DrawAxis();

    // Draw Geometry

    void DrawSphere(const Geometry::Sphere& sphere, uint32_t color, uint32_t subdivision = 12);
    void DrawCapsule(const Geometry::Capsule& capsule, uint32_t color);
    void DrawLine(const Geometry::Line& line, uint32_t color);
    void DrawRay(const Geometry::Ray& ray, uint32_t color);
    void DrawSegment(const Geometry::Segment& segment, uint32_t color);
    void DrawPlane(const Geometry::Plane& plane, uint32_t color, float size = 2.0f);
    void DrawTriangle(const Geometry::Triangle& triangle, uint32_t color);
    void DrawAABB(const Geometry::AABB& aabb, uint32_t color);
    void DrawOBB(const Geometry::OBB& obb, uint32_t color);

    void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2);
    void DrawCatmullRom(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& controlPoint3);



private:
    Matrix4x4 vpvMatrix_ = {};
    Matrix4x4 vpvMatrixInv_ = {};
};