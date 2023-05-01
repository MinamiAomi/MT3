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
	Vector3 cameraPosition;
	Vector3 cameraRotate;

	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	Matrix4x4 viewportMatrix;

	void Initalize(float windowWidth, float windowHeight);
	void UpdateMatrix();

	const Matrix4x4& GetVPVMatrix()const { return m_vpvMatrix; }

	void SetCullMode(CullMode cullMode) { m_cullMode = cullMode; }
	void SetIsWireFrame(bool isWireFrame) { m_isWireFrame = isWireFrame; }

	Vector3 Apply(const Vector3& v);

	void DrawLine(const Vector3& v1, const Vector3& v2, uint32_t color);
	void DrawTriangle(const Vector3& v1, const Vector3& v2, const Vector3& v3, uint32_t color);
	void DrawTriangle(const Vector3* vertices, uint32_t color);
	void DrawTriangle(const std::vector<Vector3>& vertices, uint32_t color);
	void DrawGrid(float width = 10.0f, uint32_t subdivision = 10);

	///
	/// Draw Geometry
	///
	void DrawSphere(const Sphere& sphere, uint32_t color, uint32_t subdivision = 12);
	void DrawLine(const Line& line, uint32_t color);
	void DrawRay(const Ray& ray, uint32_t color);
	void DrawSegment(const Segment& segment, uint32_t color);
	void DrawPlane(const Plane& plane, uint32_t color, float size = 2.0f);


private:
	Matrix4x4 m_vpvMatrix = {};
	CullMode m_cullMode = CullMode::kBack;
	bool m_isWireFrame = false;
};