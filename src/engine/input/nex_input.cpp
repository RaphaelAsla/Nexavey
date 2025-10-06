#include "nex_input.hpp"

namespace nex {
    void Input::moveInPlaneXZ(GLFWwindow* window, float dt, NexEntity& entity) {
        glm::vec3 rotate(0.0f);

        if (glfwGetKey(window, m_keys.m_look_right) == GLFW_PRESS) {
            rotate.y += 1.0f;
        }
        if (glfwGetKey(window, m_keys.m_look_left) == GLFW_PRESS) {
            rotate.y -= 1.0f;
        }
        if (glfwGetKey(window, m_keys.m_look_up) == GLFW_PRESS) {
            rotate.x += 1.0f;
        }
        if (glfwGetKey(window, m_keys.m_look_down) == GLFW_PRESS) {
            rotate.x -= 1.0f;
        }

        if (glm::length(rotate) > std::numeric_limits<float>::epsilon()) {
            entity.m_transform.m_rotation += glm::normalize(rotate) * dt * m_look_speed;
        }

        entity.m_transform.m_rotation.x = glm::clamp(entity.m_transform.m_rotation.x, -1.5f, 1.5f);
        entity.m_transform.m_rotation.y = glm::mod(entity.m_transform.m_rotation.y, glm::two_pi<float>());

        float           yaw               = entity.m_transform.m_rotation.y;
        const glm::vec3 forward_direction = {glm::sin(yaw), 0.0f, glm::cos(yaw)};
        const glm::vec3 right_direction   = {forward_direction.z, 0.0f, -forward_direction.x};
        const glm::vec3 up_direction      = {0.0f, -1.0f, 0.0f};

        glm::vec3 move_direction = {0.0f, 0.0f, 0.0f};
        if (glfwGetKey(window, m_keys.m_move_forward) == GLFW_PRESS) {
            move_direction += forward_direction;
        }
        if (glfwGetKey(window, m_keys.m_move_backward) == GLFW_PRESS) {
            move_direction -= forward_direction;
        }
        if (glfwGetKey(window, m_keys.m_move_right) == GLFW_PRESS) {
            move_direction += right_direction;
        }
        if (glfwGetKey(window, m_keys.m_move_left) == GLFW_PRESS) {
            move_direction -= right_direction;
        }
        if (glfwGetKey(window, m_keys.m_move_up) == GLFW_PRESS) {
            move_direction += up_direction;
        }
        if (glfwGetKey(window, m_keys.m_move_down) == GLFW_PRESS) {
            move_direction -= up_direction;
        }

        if (glm::length(move_direction) > std::numeric_limits<float>::epsilon()) {
            entity.m_transform.m_translation += glm::normalize(move_direction) * dt * m_move_speed;
        }
    }
};  // namespace nex
