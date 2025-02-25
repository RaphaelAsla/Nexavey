#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "Component.hpp"

namespace nex {
    class Camera : public Component {
      public:
        Camera()  = default;
        ~Camera() = default;

        void Initialize();
        void OnCreate();
        void OnDelete();
        void OnTick();
        void SetParent(Entity* parent);

        const glm::mat4& GetViewMatrix();
        const glm::mat4& GetProjectionMatrix();

        const glm::mat4 ComputeViewMatrix();
        const glm::mat4 ComputeProjectionMatrix();

        std::shared_ptr<Camera> m_active_camera;

      private:
        void ProcessInput();
        void UpdateCameraVectors();

        glm::mat4 m_view_matrix;
        glm::mat4 m_projection_matrix;
        glm::vec3 m_position;
        glm::vec3 m_right;
        glm::vec3 m_front;
        glm::vec3 m_up;

        float m_yaw;
        float m_pitch;
        float m_mouse_speed;
        float m_mouse_sensitivity;
        float m_zoom = 45.0f;
    };
}  // namespace nex
