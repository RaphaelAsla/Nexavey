#include "Camera.hpp"

#include "../../core/Input.hpp"
#include "../../core/Window.hpp"
#include "../Entity.hpp"

namespace nex {
    Camera::Camera()
        : m_position(glm::vec3(0.0f, 0.0f, 0.0f))
        , m_right(glm::vec3(1.0f, 0.0f, 0.0f))
        , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
        , m_up(glm::vec3(0.0f, 1.0f, 0.0f))
        , m_yaw(YAW)
        , m_pitch(PITCH)
        , m_zoom(ZOOM)
        , m_movement_speed(SPEED)
        , m_mouse_sensitivity(SENSITIVITY) {
        UpdateCameraVectors();
    }

    void Camera::Initialize() {
        m_position = m_parent->GetPosition();
        ComputeViewMatrix();
        ComputeProjectionMatrix();
    }

    void Camera::OnCreate() {}

    void Camera::OnDelete() {}

    void Camera::OnTick() {
        m_parent->SetPosition(m_position);
        m_view_matrix       = ComputeViewMatrix();
        m_projection_matrix = ComputeProjectionMatrix();
        UpdateCameraVectors();
        ProcessInput();
    };

    void Camera::SetParent(Entity* parent) {
        assert(parent);
        m_parent = parent;
    }

    const glm::mat4& Camera::GetViewMatrix() {
        return m_view_matrix;
    }
    const glm::mat4& Camera::GetProjectionMatrix() {
        return m_projection_matrix;
    }

    const glm::mat4 Camera::ComputeViewMatrix() {
        glm::vec3 target = m_position + m_front;
        return glm::lookAt(m_position, target, m_up);
    }

    const glm::mat4 Camera::ComputeProjectionMatrix() {
        return glm::perspective(glm::radians(m_zoom), (float)Window::GetWidth() / Window::GetHeight(), 0.1f, 100.0f);
    }

    void Camera::ProcessInput() {
        float velocity = m_movement_speed * 0.1f;

        glm::vec3 flat_front = glm::normalize(glm::vec3(m_front.x, 0.0f, m_front.z));
        glm::vec3 flat_up    = glm::normalize(glm::vec3(0.0f, m_up.y, 0.0f));

        // Forward/Backward
        if (Input::GetKey(GLFW_KEY_W)) {
            m_position += flat_front * velocity;
        }
        if (Input::GetKey(GLFW_KEY_S)) {
            m_position -= flat_front * velocity;
        }

        // Left/Right
        if (Input::GetKey(GLFW_KEY_A)) {
            m_position -= m_right * velocity;
        }
        if (Input::GetKey(GLFW_KEY_D)) {
            m_position += m_right * velocity;
        }

        // Up/Down
        if (Input::GetKey(GLFW_KEY_SPACE)) {
            m_position += flat_up * velocity;
        }
        if (Input::GetKey(GLFW_KEY_LEFT_CONTROL)) {
            m_position -= flat_up * velocity;
        }

        glm::vec2 current_mouse_pos = Input::GetMousePos();
        if (Input::m_first_mouse) {
            Input::m_last_mouse_pos = current_mouse_pos;
            Input::m_first_mouse    = false;
            return;
        }

        float xoffset           = current_mouse_pos.x - Input::m_last_mouse_pos.x;
        float yoffset           = Input::m_last_mouse_pos.y - current_mouse_pos.y;
        Input::m_last_mouse_pos = current_mouse_pos;

        xoffset *= m_mouse_sensitivity;
        yoffset *= m_mouse_sensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        // Constrain pitch
        if (m_pitch > 89.0f) {
            m_pitch = 89.0f;
        }
        if (m_pitch < -89.0f) {
            m_pitch = -89.0f;
        }

        UpdateCameraVectors();
    }

    void Camera::UpdateCameraVectors() {
        // Re-calculate the new front vector
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(front);

        // Re-calculate the right and up vector
        m_right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0f)));
        m_up    = glm::normalize(glm::cross(m_right, m_front));
    }
}  // namespace nex
