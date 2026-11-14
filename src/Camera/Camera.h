#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera configuration constants
constexpr float DEFAULT_YAW         = -90.0f;
constexpr float DEFAULT_PITCH       =  0.0f;
constexpr float DEFAULT_SPEED       =  5.0f;
constexpr float DEFAULT_SENSITIVITY =  0.1f;
constexpr float DEFAULT_FOV         =  45.0f;

class Camera {
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 3.0f, 8.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = DEFAULT_YAW,
           float pitch = DEFAULT_PITCH);

    // Returns the view matrix calculated using Euler angles and the LookAt matrix
    glm::mat4 getViewMatrix() const;

    // Returns the projection matrix
    glm::mat4 getProjectionMatrix(float aspectRatio, float nearPlane = 0.1f, float farPlane = 100.0f) const;

    // Processes keyboard input from GLFW window
    void processKeyboard(CameraMovement direction, float deltaTime);

    // Processes mouse offset from cursor callback
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // Processes mouse scroll wheel for FOV zooming
    void processMouseScroll(float yoffset);

    // Getters and Setters
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getFront() const { return m_front; }
    glm::vec3 getUp() const { return m_up; }
    glm::vec3 getRight() const { return m_right; }
    float getFov() const { return m_fov; }
    float getMovementSpeed() const { return m_movementSpeed; }

    void setPosition(const glm::vec3& position) { m_position = position; }
    void setMovementSpeed(float speed) { m_movementSpeed = speed; }

private:
    void updateCameraVectors();

    // Camera Attributes
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    // Euler Angles
    float m_yaw;
    float m_pitch;

    // Camera options
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_fov;
};
