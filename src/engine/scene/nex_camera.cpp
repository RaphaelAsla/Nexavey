#include "nex_camera.hpp"

#include <cassert>
#include <limits>

namespace nex {
    void NexCamera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
        m_projection_matrix       = glm::mat4{1.0f};
        m_projection_matrix[0][0] = 2.f / (right - left);
        m_projection_matrix[1][1] = 2.f / (bottom - top);
        m_projection_matrix[2][2] = 1.f / (far - near);
        m_projection_matrix[3][0] = -(right + left) / (right - left);
        m_projection_matrix[3][1] = -(bottom + top) / (bottom - top);
        m_projection_matrix[3][2] = -near / (far - near);
    }

    void NexCamera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
        assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
        const float tan_half_fovy = tan(fovy / 2.f);
        m_projection_matrix       = glm::mat4{0.0f};
        m_projection_matrix[0][0] = 1.f / (aspect * tan_half_fovy);
        m_projection_matrix[1][1] = 1.f / (tan_half_fovy);
        m_projection_matrix[2][2] = far / (far - near);
        m_projection_matrix[2][3] = 1.f;
        m_projection_matrix[3][2] = -(far * near) / (far - near);
    }

    void NexCamera::setViewDirection(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up) {
        const glm::vec3 w{glm::normalize(direction)};
        const glm::vec3 u{glm::normalize(glm::cross(w, up))};
        const glm::vec3 v{glm::cross(w, u)};

        m_view_matrix       = glm::mat4{1.f};
        m_view_matrix[0][0] = u.x;
        m_view_matrix[1][0] = u.y;
        m_view_matrix[2][0] = u.z;
        m_view_matrix[0][1] = v.x;
        m_view_matrix[1][1] = v.y;
        m_view_matrix[2][1] = v.z;
        m_view_matrix[0][2] = w.x;
        m_view_matrix[1][2] = w.y;
        m_view_matrix[2][2] = w.z;
        m_view_matrix[3][0] = -glm::dot(u, position);
        m_view_matrix[3][1] = -glm::dot(v, position);
        m_view_matrix[3][2] = -glm::dot(w, position);

        m_inverse_view_matrix       = glm::mat4{1.f};
        m_inverse_view_matrix[0][0] = u.x;
        m_inverse_view_matrix[0][1] = u.y;
        m_inverse_view_matrix[0][2] = u.z;
        m_inverse_view_matrix[1][0] = v.x;
        m_inverse_view_matrix[1][1] = v.y;
        m_inverse_view_matrix[1][2] = v.z;
        m_inverse_view_matrix[2][0] = w.x;
        m_inverse_view_matrix[2][1] = w.y;
        m_inverse_view_matrix[2][2] = w.z;
        m_inverse_view_matrix[3][0] = position.x;
        m_inverse_view_matrix[3][1] = position.y;
        m_inverse_view_matrix[3][2] = position.z;
    }

    void NexCamera::setViewTarget(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) {
        setViewDirection(position, target - position, up);
    }

    void NexCamera::setViewYXZ(const glm::vec3& position, const glm::vec3& rotation) {
        const float     c3 = glm::cos(rotation.z);
        const float     s3 = glm::sin(rotation.z);
        const float     c2 = glm::cos(rotation.x);
        const float     s2 = glm::sin(rotation.x);
        const float     c1 = glm::cos(rotation.y);
        const float     s1 = glm::sin(rotation.y);
        const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
        const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
        const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
        m_view_matrix       = glm::mat4{1.f};
        m_view_matrix[0][0] = u.x;
        m_view_matrix[1][0] = u.y;
        m_view_matrix[2][0] = u.z;
        m_view_matrix[0][1] = v.x;
        m_view_matrix[1][1] = v.y;
        m_view_matrix[2][1] = v.z;
        m_view_matrix[0][2] = w.x;
        m_view_matrix[1][2] = w.y;
        m_view_matrix[2][2] = w.z;
        m_view_matrix[3][0] = -glm::dot(u, position);
        m_view_matrix[3][1] = -glm::dot(v, position);
        m_view_matrix[3][2] = -glm::dot(w, position);

        m_inverse_view_matrix       = glm::mat4{1.f};
        m_inverse_view_matrix[0][0] = u.x;
        m_inverse_view_matrix[0][1] = u.y;
        m_inverse_view_matrix[0][2] = u.z;
        m_inverse_view_matrix[1][0] = v.x;
        m_inverse_view_matrix[1][1] = v.y;
        m_inverse_view_matrix[1][2] = v.z;
        m_inverse_view_matrix[2][0] = w.x;
        m_inverse_view_matrix[2][1] = w.y;
        m_inverse_view_matrix[2][2] = w.z;
        m_inverse_view_matrix[3][0] = position.x;
        m_inverse_view_matrix[3][1] = position.y;
        m_inverse_view_matrix[3][2] = position.z;
    }
}  // namespace nex
