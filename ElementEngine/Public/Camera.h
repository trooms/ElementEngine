#pragma once
#include <mat4x4.hpp>
#include <vec3.hpp>

#define GLM_ENABLE_EXPERIMENTAL

class Camera
{
public:
    Camera();
    Camera(const Camera &);
    ~Camera();

    void SetPosition(float, float, float);
    void SetRotation(float, float, float);

    glm::vec3 GetPosition();
    glm::vec3 GetRotation();

    void Render();
    void GetViewMatrix(glm::mat4x4 &);

private:
    float m_positionX, m_positionY, m_positionZ;
    float m_rotationX, m_rotationY, m_rotationZ;
    glm::mat4x4 m_viewMatrix;
};