#include <Camera.h>
#include <gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>

Camera::Camera()
{
    m_positionX = 0.0f;
    m_positionY = 0.0f;
    m_positionZ = 0.0f;

    m_rotationX = 0.0f;
    m_rotationY = 0.0f;
    m_rotationZ = 0.0f;
}
Camera::Camera(const Camera &)
{
}
Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
    m_positionX = x;
    m_positionY = y;
    m_positionZ = z;
}

void Camera::SetRotation(float x, float y, float z)
{
    m_rotationX = x;
    m_rotationY = y;
    m_rotationZ = z;
}

glm::vec3 Camera::GetPosition()
{
    return glm::vec3(m_positionX, m_positionY, m_positionZ);
}
glm::vec3 Camera::GetRotation()
{
    return glm::vec3(m_rotationX, m_rotationY, m_rotationZ);
}

void Camera::Render()
{
    // Setup the vector that points upwards.
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Setup the position of the camera in the world.
    glm::vec3 position = glm::vec3(m_positionX, m_positionY, m_positionZ);

    // Setup where the camera is looking by default.
    glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, 1.0f);

    // Convert rotations from degrees to radians.
    float pitch = glm::radians(m_rotationX);
    float yaw = glm::radians(m_rotationY);
    float roll = glm::radians(m_rotationZ);

    // Create the rotation matrix using yaw (Y), pitch (X), and roll (Z).
    glm::mat4 rotationMatrix = glm::yawPitchRoll(yaw, pitch, roll);

    // Rotate the lookAt and up vectors by the rotation matrix.
    glm::vec3 rotatedLookAt = glm::vec3(rotationMatrix * glm::vec4(lookAt, 0.0f));
    glm::vec3 rotatedUp = glm::vec3(rotationMatrix * glm::vec4(up, 0.0f));

    // Translate the rotated lookAt vector to the camera's position.
    glm::vec3 target = position + rotatedLookAt;

    // Create the view matrix using glm::lookAt (left-handed coordinate system).
    m_viewMatrix = glm::lookAtLH(position, target, rotatedUp);
}

void Camera::GetViewMatrix(glm::mat4x4 &viewMatrix)
{
    viewMatrix = m_viewMatrix;
}