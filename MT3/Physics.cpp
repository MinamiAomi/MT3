#include "Physics.h"

#include <ImGuiManager.h>

void PhysicsObject::AddForce(const Vector3& force) {
    acceleration += force * (1.0f / mass);
}

void PhysicsObject::UpdatePosition(float deltaTime) {
    prePosition = position;
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;
    acceleration = Vector3Zero;
}

bool PhysicsObject::ShowUI(const char* label) {
    if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat3("Position", &position.x, 0.1f);
        ImGui::DragFloat3("Velocity", &velocity.x, 0.1f);
        ImGui::DragFloat3("Acceleration", &acceleration.x, 0.1f);
        ImGui::DragFloat("Mass", &mass, 0.1f);
        ImGui::TreePop();
        return true;
    }
    return false;
}

Vector3 Spring::ComputeForce(const PhysicsObject& physicsObject) {
    Vector3 difference = physicsObject.position - anchor;
    float length = Length(difference);
    if (length == 0.0f) {
        return {};
    }
    Vector3 direction = difference * (1.0f / length);
    Vector3 restPosition = anchor + direction * naturalLength;
    // 変位ベクトル
    Vector3 displacement = length * (physicsObject.position - restPosition);
    // 復元力
    Vector3 restoringForce = -stiffness * displacement;
    // 減衰力
    Vector3 dempingForce = -damping * physicsObject.velocity;
    return restoringForce + dempingForce;
}

bool Spring::ShowUI(const char* label) {
    if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat3("Anchor", &anchor.x, 0.1f);
        ImGui::DragFloat("NaturalLength", &naturalLength, 0.1f);
        ImGui::DragFloat("Stiffness", &stiffness, 0.1f);
        ImGui::DragFloat("Damping", &damping, 0.01f);
        ImGui::TreePop();
        return true;
    }
    return false;
}

void Pendulm::UpdateAngle(float deltaTime, float gravityAcceleration) {
    angulerVelocity += -(gravityAcceleration / length) * std::sin(angle) * deltaTime;
    angle += angulerVelocity * deltaTime;
}

Vector3 Pendulm::ComputePosition() {
    Vector3 point{};
    point.x = anchor.x + std::sin(angle) * length;
    point.y = anchor.y - std::cos(angle) * length;
    point.z = anchor.z;
    return point;
}

void ConicalPendulm::UpdateAngle(float deltaTime, float gravityAcceleration) {
    angulerVelocity = std::sqrt(gravityAcceleration / (length * std::cos(halfApexAngle)));
    angle += angulerVelocity * deltaTime;
}

Vector3 ConicalPendulm::ComputePosition() {
    float radius = std::sin(halfApexAngle) * length;
    float height = std::cos(halfApexAngle) * length;
    Vector3 point{};
    point.x = anchor.x + std::cos(angle) * radius;
    point.y = anchor.y - height;
    point.z = anchor.z - std::sin(angle) * radius;
    return point;
}
