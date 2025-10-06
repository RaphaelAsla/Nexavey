#pragma once

#include <memory>
#include <unordered_map>

#include "nex_mesh.hpp"
#include "../graphics/nex_texture.hpp"

namespace nex {
    struct TransformComponent {
        glm::vec3 m_translation = {};
        glm::vec3 m_scale       = {1.0f, 1.0f, 1.0f};
        glm::vec3 m_rotation    = {};

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    struct PointLightComponent {
        float m_intensity = 1.0f;
    };

    class NexEntity {
      public:
        using id_t = unsigned int;
        using Map  = std::unordered_map<id_t, NexEntity>;

        static NexEntity create() {
            static id_t current_id = 0;
            return NexEntity(current_id++);
        }

        static NexEntity makePointLight(float intesity = 10.f, float radius = 0.1f, glm::vec3 color = {1.0f, 1.0f, 1.0f});

        NexEntity(const NexEntity&)            = delete;
        NexEntity& operator=(const NexEntity&) = delete;
        NexEntity(NexEntity&&)                 = default;
        NexEntity& operator=(NexEntity&&)      = default;

        id_t getId() const {
            return m_id;
        }

        int                m_material_index;
        glm::vec3          m_color;
        TransformComponent m_transform;

        // Optional components
        std::shared_ptr<NexMesh>             m_model       = {};
        std::unique_ptr<PointLightComponent> m_point_light = {};
        std::shared_ptr<NexTexture>          m_texture     = {};

      private:
        NexEntity(id_t id) : m_id(id) {}
        id_t m_id;
    };
};  // namespace nex
