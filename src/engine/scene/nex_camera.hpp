#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace nex {
    class NexCamera {
      public:
        void setOrthographicProjection(float left, float right, float bottom, float top, float near, float far);
        void setPerspectiveProjection(float fov, float aspect_ratio, float near, float far);
        void setViewDirection(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up = {0.0f, -1.0f, 0.0f});
        void setViewTarget(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up = {0.0f, -1.0f, 0.0f});
        void setViewYXZ(const glm::vec3& position, const glm::vec3& rotation);

        const glm::mat4& getProjectionMatrix() const {
            return m_projection_matrix;
        }

        const glm::mat4& getViewMatrix() const {
            return m_view_matrix;
        }

        const glm::mat4& getInverseViewMatrix() const {
            return m_inverse_view_matrix;
        }

        const glm::vec3 getPosition() const {
            return glm::vec3(m_inverse_view_matrix[3]);
        }

      private:
        glm::mat4 m_projection_matrix   = {1.0f};
        glm::mat4 m_view_matrix         = {1.0f};
        glm::mat4 m_inverse_view_matrix = {1.0f};
    };
}  // namespace nex
