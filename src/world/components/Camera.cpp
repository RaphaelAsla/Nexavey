#include "Camera.hpp"

#include "../../core/Input.hpp"
#include "../../core/Window.hpp"
#include "../Entity.hpp"

namespace nex {
    Camera::Camera(const glm::vec3& position) : m_position(position) {};

    void Camera::Initialize() {
        ComputeViewMatrix();
        ComputeProjectionMatrix();
    }

    void Camera::OnCreate() {}

    void Camera::OnDelete() {}

    void Camera::OnTick() {
        m_parent->SetPosition(m_position);
        m_view_matrix = ComputeViewMatrix();
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
        return glm::lookAt(m_position, m_position * glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    const glm::mat4 Camera::ComputeProjectionMatrix() {
        return glm::perspective(glm::radians(zoom), (float)Window::GetWidth() / Window::GetHeight(), 0.1f, 100.0f);
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
    }

    void Camera::UpdateCameraVectors() {
        // also re-calculate the Right and Up vector
        m_right = glm::normalize(glm::cross(
            glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    }
}  // namespace nex
