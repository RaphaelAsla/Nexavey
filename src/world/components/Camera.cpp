#include "Camera.hpp"

#include "../../core/Input.hpp"
#include "../../core/Logger.hpp"
#include "../../core/Window.hpp"
#include "../Entity.hpp"

namespace nex {
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
        if (Input::GetKey(GLFW_KEY_W)) {
            m_position += glm::vec3(0.0f, 0.0f, -1.0f) * 0.1f;
        }
        if (Input::GetKey(GLFW_KEY_S)) {
            m_position -= glm::vec3(0.0f, 0.0f, -1.0f) * 0.1f;
        }
        if (Input::GetKey(GLFW_KEY_D)) {
            m_position += m_right * 0.1f;
        }
        if (Input::GetKey(GLFW_KEY_A)) {
            m_position -= m_right * 0.1f;
        }

        // Handle mouse input
        glm::vec2 current_mouse_pos = Input::GetMousePos();
        if (Input::m_first_mouse) {
            // Skip delta calculation on the first frame
            Input::m_last_mouse_pos = current_mouse_pos;
            Input::m_first_mouse    = false;
        }

        glm::vec2 delta         = current_mouse_pos - Input::m_last_mouse_pos;
        Input::m_last_mouse_pos = current_mouse_pos;

        // Apply mouse movement to adjust camera orientation
        float sensitivity = 0.1f;  // Adjust sensitivity as needed
        delta *= sensitivity;

        // Update camera orientation (pitch and yaw)
        m_pitch += -delta.y;  // Invert y-axis movement for natural control
        m_yaw += delta.x;

        // Clamp pitch to avoid gimbal lock
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;

        UpdateCameraVectors();
    }

    void Camera::UpdateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(front);

        m_right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0f)));
        m_up    = glm::normalize(glm::cross(m_right, m_front));
    }
}  // namespace nex
