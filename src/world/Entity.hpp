#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

#include "components/Component.hpp"

namespace nex {
    class Entity {
      public:
        Entity()  = default;
        ~Entity() = default;

        void Initialize();
        void OnDelete();
        void Tick();

        void AddComponent(const std::shared_ptr<Component>& component);

        void SetPosition(const glm::vec3& position);
        void SetScale(const glm::vec3& scale);
        void SetRotation(const float rads, const glm::vec3& direction);
        void Translate(const glm::vec3& delta);
        void UpdateTransform();

        glm::vec3 GetPosition();
        glm::vec3 GetScale();
        glm::mat4 GetRotation();
        glm::mat4 GetTransformMatrix();

      private:
        std::vector<std::shared_ptr<Component>> m_componenets;
        glm::vec3                               m_position         = glm::vec3(0.0f);
        glm::vec3                               m_scale            = glm::vec3(1.0f);
        glm::mat4                               m_rotation         = glm::mat4(1.0f);
        glm::mat4                               m_transform_martix = glm::mat4(1.0f);
    };
}  // namespace nex
