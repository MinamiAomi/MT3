#pragma once

#include "MyMath.h"

class PhysicsObject {
public:
    void AddForce(const Vector3& force);
    void UpdatePosition(float deltaTime);

    bool ShowUI(const char* label);

    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    float mass;
};

class Spring {
public:
    Vector3 ComputeForce(const PhysicsObject& physicsObject);

    bool ShowUI(const char* label);

    Vector3 anchor;
    float naturalLength; // 自然長
    float stiffness; // 硬さ
    float damping; // 減衰係数
};

class Pendulm {
public:
    void UpdateAngle(float deltaTime, float gravityAcceleration);
    Vector3 ComputePosition();

    Vector3 anchor;
    float length;
    float angle;
    float angulerVelocity;
    float angulerAcceleration;
};