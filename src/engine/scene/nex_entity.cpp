#include "nex_entity.hpp"

namespace nex {
    glm::mat4 TransformComponent::mat4() {
        const float c3 = glm::cos(m_rotation.z);
        const float s3 = glm::sin(m_rotation.z);
        const float c2 = glm::cos(m_rotation.x);
        const float s2 = glm::sin(m_rotation.x);
        const float c1 = glm::cos(m_rotation.y);
        const float s1 = glm::sin(m_rotation.y);

        return glm::mat4{{
                             m_scale.x * (c1 * c3 + s1 * s2 * s3),
                             m_scale.x * (c2 * s3),
                             m_scale.x * (c1 * s2 * s3 - c3 * s1),
                             0.0f,
                         },
                         {
                             m_scale.y * (c3 * s1 * s2 - c1 * s3),
                             m_scale.y * (c2 * c3),
                             m_scale.y * (c1 * c3 * s2 + s1 * s3),
                             0.0f,
                         },
                         {
                             m_scale.z * (c2 * s1),
                             m_scale.z * (-s2),
                             m_scale.z * (c1 * c2),
                             0.0f,
                         },
                         {m_translation.x, m_translation.y, m_translation.z, 1.0f}};
    }

    glm::mat3 TransformComponent::normalMatrix() {
        const float c3 = glm::cos(m_rotation.z);
        const float s3 = glm::sin(m_rotation.z);
        const float c2 = glm::cos(m_rotation.x);
        const float s2 = glm::sin(m_rotation.x);
        const float c1 = glm::cos(m_rotation.y);
        const float s1 = glm::sin(m_rotation.y);

        glm::vec3 inv_scale = {1.0f / m_scale.x, 1.0f / m_scale.y, 1.0f / m_scale.z};

        return glm::mat3{
            {
                inv_scale.x * (c1 * c3 + s1 * s2 * s3),
                inv_scale.x * (c2 * s3),
                inv_scale.x * (c1 * s2 * s3 - c3 * s1),
            },
            {
                inv_scale.y * (c3 * s1 * s2 - c1 * s3),
                inv_scale.y * (c2 * c3),
                inv_scale.y * (c1 * c3 * s2 + s1 * s3),
            },
            {
                inv_scale.z * (c2 * s1),
                inv_scale.z * (-s2),
                inv_scale.z * (c1 * c2),
            },
        };
    }

    NexEntity NexEntity::makePointLight(float intesity, float radius, glm::vec3 color) {
        NexEntity entity                  = NexEntity::create();
        entity.m_color                    = color;
        entity.m_transform.m_scale.x      = radius;
        entity.m_point_light              = std::make_unique<PointLightComponent>();
        entity.m_point_light->m_intensity = intesity;
        return entity;
    }

};  // namespace nex
