#include "ReneringPipeline.h"
#include <Novice.h>
#include <cassert>
#include <algorithm>

Vector3 RenderingPipeline::Apply(const Vector3& v) {
	return Transform(v, m_vpvMatrix);
}

void RenderingPipeline::DrawLine(const Vector3& v1, const Vector3& v2, uint32_t color) {
	Vector3 a = Apply(v1);
	Vector3 b = Apply(v2);

	Novice::DrawLine(static_cast<int>(a.x), static_cast<int>(a.y),
		static_cast<int>(b.x), static_cast<int>(b.y), color);
}

void RenderingPipeline::DrawTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3, uint32_t color) {
	Vector3 a = Apply(v1);
	Vector3 b = Apply(v2);
	Vector3 c = Apply(v3);
	// ワイヤーフレーム表示
	if (m_isWireFrame) {
		Novice::DrawTriangle(
			static_cast<int>(a.x), static_cast<int>(a.y),
			static_cast<int>(b.x), static_cast<int>(b.y),
			static_cast<int>(c.x), static_cast<int>(c.y),
			color, kFillModeWireFrame);
		return;
	}

	switch (m_cullMode) {
	case CullMode::kNone:
		Novice::DrawTriangle(
			static_cast<int>(a.x), static_cast<int>(a.y),
			static_cast<int>(b.x), static_cast<int>(b.y),
			static_cast<int>(c.x), static_cast<int>(c.y),
			color, kFillModeSolid);
		break;
	default:
	case CullMode::kBack:
	{
		Vector3 triangleNormal = Cross(b - a, c - b);
		// 表なら
		if (Dot({ 0.0f,0.0f,1.0f }, triangleNormal) <= 0) {
			Novice::DrawTriangle(
				static_cast<int>(a.x), static_cast<int>(a.y),
				static_cast<int>(b.x), static_cast<int>(b.y),
				static_cast<int>(c.x), static_cast<int>(c.y),
				color, kFillModeSolid);
		}
		break;
	}
	case CullMode::kFront:
	{
		Vector3 triangleNormal = Cross(b - a, c - b);
		// 表なら
		if (Dot({ 0.0f,0.0f,-1.0f }, triangleNormal) <= 0) {
			Novice::DrawTriangle(
				static_cast<int>(a.x), static_cast<int>(a.y),
				static_cast<int>(b.x), static_cast<int>(b.y),
				static_cast<int>(c.x), static_cast<int>(c.y),
				color, kFillModeSolid);
		}
		break;
	}
	}
}

void RenderingPipeline::DrawTriangle(const Vector3* vertices, uint32_t color) {
	DrawTriangle(vertices[0], vertices[1], vertices[2], color);
}

void RenderingPipeline::DrawTriangle(const std::vector<Vector3>& vertices, uint32_t color) {
	DrawTriangle(vertices[0], vertices[1], vertices[2], color);
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

void RenderingPipeline::DrawSphere(const Sphere& sphere, uint32_t color, uint32_t subdivision) {
	assert(subdivision >= 3);

	const float kLatEvery = Math::Pi / subdivision;
	const float kLonEvery = Math::TwoPi / subdivision;

	Matrix4x4 worldMatrix = MakeAffineMatrix({ sphere.radius, sphere.radius,sphere.radius }, { 0.0f,0.0f,0.0f }, sphere.center);

	// θとΦから頂点座標を計算
	auto CalcVertex = [&](float theta, float phi) {
		Vector3 v{};
		v.x = std::cos(theta) * std::cos(phi);
		v.y = std::sin(theta);
		v.z = std::cos(theta) * std::sin(phi);
		v = v * sphere.radius + sphere.center;
		return v;
	};

	Vector3 a{}, b{}, c{}, d{};

	for (uint32_t latIndex = 0; latIndex < subdivision; ++latIndex) {
		float lat = -Math::HalfPi + latIndex * kLatEvery;
		for (uint32_t lonIndex = 0; lonIndex < subdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;

			a = CalcVertex(lat, lon);
			b = CalcVertex(lat + kLatEvery, lon);
			c = CalcVertex(lat, lon + kLonEvery);

			if (m_isWireFrame) {
				DrawLine(a, b, color);
				DrawLine(a, c, color);
				continue;
			}

			d = CalcVertex(lat + kLatEvery, lon + kLonEvery);

			DrawTriangle(a, b, c, color);
			DrawTriangle(d, c, b, color);
		}
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

void RenderingPipeline::UpdateMatrix() {
	Matrix4x4 cameraMat = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraPosition);
	viewMatrix = Inverse(cameraMat);

	m_vpvMatrix = MakeIdentityMatrix();
	m_vpvMatrix *= viewMatrix;
	m_vpvMatrix *= projectionMatrix;
	m_vpvMatrix *= viewportMatrix;
}

void RenderingPipeline::SetCullMode(CullMode cullMode) { m_cullMode = cullMode; }

void RenderingPipeline::SetIsWireFrame(bool isWireFrame) { m_isWireFrame = isWireFrame; }