#include "ReneringPipeline.h"
#include <Novice.h>
#include <cassert>
#include <algorithm>

using namespace Geometry;

void RenderingPipeline::Initalize(float windowWidth, float windowHeight) {
    // ビュー行列をセット
    cameraRotate = { 15.0f * Math::ToRadian, 0.0f, 0.0f };
    Vector3 forward = GetZAxis(MakeRotateXYZMatrix(cameraRotate));
    targetDistance = 6.0f;
    cameraPosition = -forward * targetDistance;
    // 射影行列をセット
    projectionMatrix = MakePerspectiveFovMatrix(0.45f, windowWidth / windowHeight, 0.1f, 100.0f);
    // ビューポート行列をセット
    viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, windowWidth, windowHeight, 0.0f, 1.0f);
    // 行列を合成
    UpdateMatrix();
}

void RenderingPipeline::UpdateMatrix() {
    Matrix4x4 cameraMat = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraPosition);
    viewMatrix = Inverse(cameraMat);

    vpvMatrix_ = MakeIdentityMatrix();
    vpvMatrix_ *= viewMatrix;
    vpvMatrix_ *= projectionMatrix;
    vpvMatrix_ *= viewportMatrix;

    vpvMatrixInv_ = Inverse(vpvMatrix_);
}

void RenderingPipeline::ScreenDrawLine(const Vector3& v1, const Vector3& v2, uint32_t color) {
    Novice::DrawLine(static_cast<int>(v1.x), static_cast<int>(v1.y),
        static_cast<int>(v2.x), static_cast<int>(v2.y), color);
}

void RenderingPipeline::DrawLine(const Vector3& v1, const Vector3& v2, uint32_t color) {
    Vector3 a = Apply(v1);
    Vector3 b = Apply(v2);

    Novice::DrawLine(static_cast<int>(a.x), static_cast<int>(a.y),
        static_cast<int>(b.x), static_cast<int>(b.y), color);
}

void RenderingPipeline::DrawGrid(float width, uint32_t subdivision) {
    const float kGridHalfWidth = width * 0.5f;
    const float kGridEvery = (width) / static_cast<float>(subdivision);

    for (uint32_t index = 0; index <= subdivision; ++index) {
        float gridPos = index * kGridEvery - kGridHalfWidth;
        Vector3 x1 = { kGridHalfWidth,0.0f, gridPos };
        Vector3 x2 = { -kGridHalfWidth,0.0f, gridPos };
        Vector3 z1 = { gridPos, 0.0f, kGridHalfWidth };
        Vector3 z2 = { gridPos, 0.0f, -kGridHalfWidth };
        uint32_t color = index == (subdivision * 0.5f) ? 0x000000FF : 0xAAAAAAFF;
        DrawLine(x1, x2, color);
        DrawLine(z1, z2, color);
    }
}

void RenderingPipeline::DrawBox(const Matrix4x4& world, uint32_t color) {
    std::vector<Vector3> vertices;
    GetVertices({ -0.5f,-0.5f,-0.5f }, { 0.5f,0.5f,0.5 }, vertices);
    for (auto& vertex : vertices) {
        vertex = vertex * world;
        vertex = Apply(vertex);
    }
    for (uint32_t i = 0, j = 0; i < 4; ++i) {
        j = (i + 1) % 4;
        ScreenDrawLine(vertices[i], vertices[j], color);
        ScreenDrawLine(vertices[i], vertices[i + 4], color);
        ScreenDrawLine(vertices[i + 4], vertices[j + 4], color);
    }
}

void RenderingPipeline::DrawAxis() {
    Vector3 translate = Transform({ 0.0f,0.0f,-10.0f }, MakeRotateXYZMatrix(cameraRotate));
    Matrix4x4 vpvMatrix = MakeIdentityMatrix();
    vpvMatrix *= Inverse(MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, translate));
    vpvMatrix *= projectionMatrix;
    vpvMatrix *= viewportMatrix;

    Vector3 o = Transform({ 0.0f,0.0f,0.0f }, vpvMatrix);
    Vector3 x = Transform({ 0.3f,0.0f,0.0f }, vpvMatrix);
    Vector3 y = Transform({ 0.0f,0.3f,0.0f }, vpvMatrix);
    Vector3 z = Transform({ 0.0f,0.0f,0.3f }, vpvMatrix);

    Vector3 offset = { -590.0f, 310.0f, 0.0f };
    o += offset;
    x += offset;
    y += offset;
    z += offset;

    ScreenDrawLine(o, x, RED);
    ScreenDrawLine(o, y, GREEN);
    ScreenDrawLine(o, z, BLUE);
}

void RenderingPipeline::DrawSphere(const Sphere& sphere, uint32_t color, uint32_t subdivision) {
    assert(subdivision >= 3);

    const float kLatEvery = Math::Pi / subdivision;
    const float kLonEvery = Math::TwoPi / subdivision;

    //Matrix4x4 worldMatrix = MakeAffineMatrix({ sphere.radius, sphere.radius,sphere.radius }, { 0.0f,0.0f,0.0f }, sphere.center);

    // θとΦから頂点座標を計算
    auto CalcVertex = [&](float theta, float phi) {
        Vector3 v{};
        v.x = std::cos(theta) * std::cos(phi);
        v.y = std::sin(theta);
        v.z = std::cos(theta) * std::sin(phi);
        v = v * sphere.radius + sphere.center;
        return Apply(v);
    };

    Vector3 a{}, b{}, c{}, d{};

    for (uint32_t latIndex = 0; latIndex < subdivision; ++latIndex) {
        float lat = -Math::HalfPi + latIndex * kLatEvery;
        for (uint32_t lonIndex = 0; lonIndex < subdivision; ++lonIndex) {
            float lon = lonIndex * kLonEvery;

            a = CalcVertex(lat, lon);
            b = CalcVertex(lat + kLatEvery, lon);
            c = CalcVertex(lat, lon + kLonEvery);

            ScreenDrawLine(a, b, color);
            ScreenDrawLine(a, c, color);
        }
    }
}

void RenderingPipeline::DrawCapsule(const Geometry::Capsule& capsule, uint32_t color) {
    // 半球部の球分割数
    const uint32_t kLatDiv = 7;
    const uint32_t kLonDiv = 4;
    const float kLatEvery = Math::HalfPi / kLatDiv;
    const float kLonEvery = Math::TwoPi / kLonDiv;

    Matrix4x4 rotateMatrix = MakeIdentityMatrix();
    if (std::abs(Dot(Normalize(capsule.segment.diff), Vector3UnitX)) < 1.0f - 0.0001f) {
        Vector3 y = -Normalize(capsule.segment.diff);
        Vector3 x = Normalize(Cross(-Vector3UnitX, y));
        Vector3 z = -Cross(y, x);
        SetXAxis(rotateMatrix, x);
        SetYAxis(rotateMatrix, y);
        SetZAxis(rotateMatrix, z);
    }
    else {
        rotateMatrix = MakeRotateXMatrix(90.0f * Math::ToRadian);
    }

    Vector3 center = capsule.segment.origin + capsule.segment.diff * 0.5f;
    Vector3 direction = (capsule.segment.diff != Vector3Zero) ? Normalize(capsule.segment.diff) : Vector3Zero;
    float cylinderHalfHeight = Length(capsule.segment.diff) * 0.5f;
    Vector3 upOffset = direction * -cylinderHalfHeight + center;
    Vector3 downOffset = direction * cylinderHalfHeight + center;

    // θとΦから頂点座標を計算
    auto CalcVertex = [&](float theta, float phi) {
        Vector3 v{};
        v.x = std::cos(theta) * std::cos(phi);
        v.y = std::sin(theta);
        v.z = std::cos(theta) * std::sin(phi);
        v *= capsule.radius;
        v = v * rotateMatrix;
        return v;
    };

    for (uint32_t lonIndex = 0; lonIndex < kLonDiv; ++lonIndex) {
        float lon = lonIndex * kLonEvery;
        for (uint32_t latIndex = 0; latIndex < kLatDiv; ++latIndex) {
            float latUp = Math::HalfPi - latIndex * kLatEvery;
            float latDown = -Math::HalfPi + latIndex * kLatEvery;

            Vector3 a = CalcVertex(latUp, lon) + upOffset;
            Vector3 b = CalcVertex(latUp - kLatEvery, lon) + upOffset;
            a = Apply(a);
            b = Apply(b);

            Vector3 c = CalcVertex(latDown, lon) + downOffset;
            Vector3 d = CalcVertex(latDown + kLatEvery, lon) + downOffset;
            c = Apply(c);
            d = Apply(d);

            ScreenDrawLine(a, b, color);
            ScreenDrawLine(c, d, color);
        }

        Vector3 p = CalcVertex(0.0f, lon);
        DrawLine(p + upOffset, p + downOffset, color);
    }
    
}

void RenderingPipeline::DrawLine(const Line& line, uint32_t color) {
    DrawLine(line.origin, line.origin + line.diff, color);
}

void RenderingPipeline::DrawRay(const Ray& ray, uint32_t color) {
    DrawLine(ray.origin, ray.origin + ray.diff, color);
}

void RenderingPipeline::DrawSegment(const Segment& segment, uint32_t color) {
    DrawLine(segment.origin, segment.origin + segment.diff, color);
}

void RenderingPipeline::DrawPlane(const Plane& plane, uint32_t color, float size) {
    Vector3 center = plane.distance * plane.normal;

    auto Perpendicular = [](const Vector3& v) {
        if (v.x != 0.0f || v.y != 0.0f) {
            return Vector3(-v.y, v.x, 0.0f);
        }
        return Vector3(0.0f, -v.z, v.y);
    };

    Vector3 vertices[4] = {};
    vertices[0] = Normalize(Perpendicular(plane.normal));
    vertices[1] = Cross(plane.normal, vertices[0]);
    vertices[2] = -vertices[0];
    vertices[3] = -vertices[1];

    float scale = size * 0.5f;

    for (auto& vertex : vertices) {
        vertex *= scale;
        vertex += center;
        vertex = Apply(vertex);
    }

    uint32_t i = 0;
    uint32_t j = 0;
    for (; i < 4; i++) {
        j = (i + 1) % 4;
        ScreenDrawLine(vertices[i], vertices[j], color);
    }
}

void RenderingPipeline::DrawTriangle(const Triangle& triangle, uint32_t color) {
    const Vector3 vertices[] = {
        Apply(triangle.vertices[0]),
        Apply(triangle.vertices[1]),
        Apply(triangle.vertices[2]),
    };
    Novice::DrawTriangle(
        static_cast<int>(vertices[0].x), static_cast<int>(vertices[0].y),
        static_cast<int>(vertices[1].x), static_cast<int>(vertices[1].y),
        static_cast<int>(vertices[2].x), static_cast<int>(vertices[2].y), color, kFillModeWireFrame);
}

void RenderingPipeline::DrawAABB(const AABB& aabb, uint32_t color) {
    std::vector<Vector3> vertices;
    GetVertices(aabb.min, aabb.max, vertices);
    for (auto& vertex : vertices) {
        vertex = Apply(vertex);
    }
    uint32_t i{}, j{};
    for (i = 0; i < 4; ++i) {
        j = (i + 1) % 4;
        ScreenDrawLine(vertices[i], vertices[j], color);
        ScreenDrawLine(vertices[i], vertices[i + 4], color);
        ScreenDrawLine(vertices[i + 4], vertices[j + 4], color);
    }
}

void RenderingPipeline::DrawOBB(const OBB& obb, uint32_t color) {
    std::vector<Vector3> vertices;
    GetVertices(obb, vertices);
    for (auto& vertex : vertices) {
        vertex = Apply(vertex);
    }
    uint32_t i{}, j{};
    for (i = 0; i < 4; ++i) {
        j = (i + 1) % 4;
        ScreenDrawLine(vertices[i], vertices[j], color);
        ScreenDrawLine(vertices[i], vertices[i + 4], color);
        ScreenDrawLine(vertices[i + 4], vertices[j + 4], color);
    }
}

void RenderingPipeline::DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2) {
    const uint32_t kSegmentCount = 100;
    const uint32_t kVertexCount = kSegmentCount + 1;

    Vector3 vertices[kVertexCount]{};
    for (uint32_t i = 0; i < kVertexCount; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(kSegmentCount);
        Vector3 p0p1 = Lerp(controlPoint0, controlPoint1, t);
        Vector3 p1p2 = Lerp(controlPoint1, controlPoint2, t);
        vertices[i] = Lerp(p0p1, p1p2, t);
        vertices[i] = Apply(vertices[i]);
    }

    for (uint32_t i = 0; i < kSegmentCount; ++i) {
        ScreenDrawLine(vertices[i], vertices[i + 1], WHITE);
    }
    DrawSphere({ controlPoint0,0.01f }, BLACK, 3);
    DrawSphere({ controlPoint1,0.01f }, BLACK, 3);
    DrawSphere({ controlPoint2,0.01f }, BLACK, 3);
}

void RenderingPipeline::DrawCatmullRom(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& controlPoint3) {
    const uint32_t kSectionCount = 3;
    const uint32_t kSegmentCountInSection = 50;
    const uint32_t kVertexCount = kSegmentCountInSection * kSectionCount + 1;

    Vector3 vertices[kVertexCount]{};
    for (uint32_t i = 0; i < kVertexCount; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(kSegmentCountInSection);
        if (t <= 1.0f) {
            vertices[i] = CatmullRomSpline(controlPoint0, controlPoint0, controlPoint1, controlPoint2, t);
        }
        else if (t <= 2.0f) {
            vertices[i] = CatmullRomSpline(controlPoint0, controlPoint1, controlPoint2, controlPoint3, t - 1.0f);
        }
        else {
            vertices[i] = CatmullRomSpline(controlPoint1, controlPoint2, controlPoint3, controlPoint3, t - 2.0f);
        }

        vertices[i] = Apply(vertices[i]);
    }

    for (uint32_t i = 0; i < kSectionCount; ++i) {
        for (uint32_t j = 0; j < kSegmentCountInSection; ++j) {
            uint32_t vertexIndex = i * kSegmentCountInSection + j;
            ScreenDrawLine(vertices[vertexIndex], vertices[vertexIndex + 1], WHITE);
        }
    }
    DrawSphere({ controlPoint0,0.01f }, BLACK, 3);
    DrawSphere({ controlPoint1,0.01f }, BLACK, 3);
    DrawSphere({ controlPoint2,0.01f }, BLACK, 3);
    DrawSphere({ controlPoint3,0.01f }, BLACK, 3);
}
