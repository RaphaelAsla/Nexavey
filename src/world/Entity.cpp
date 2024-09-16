#include "Entity.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace nex {
    void Entity::Initialize() {
        for (const auto& component : m_componenets) {
            component->Initialize();
        }
    }

    void Entity::Tick() {
        for (const auto& component : m_componenets) {
            component->OnTick();
        }
    }

    void Entity::OnDelete() {
        for (const auto& component : m_componenets) {
            component->OnDelete();
        }
    }

    void Entity::AddComponent(const std::shared_ptr<Component>& component) {
        component->SetParent(this);
        m_componenets.emplace_back(component);
    }

    void Entity::UpdateTransform() {
        m_transform_martix = glm::mat4(1.0f);
        m_transform_martix = glm::translate(m_transform_martix, m_position);
        m_transform_martix = glm::scale(m_transform_martix, m_scale);
        m_transform_martix *= m_rotation;
    }

    void Entity::SetPosition(const glm::vec3& position) {
        if (m_position == position) {
            return;
        }
        m_position = position;
        UpdateTransform();
    }

    void Entity::SetScale(const glm::vec3& scale) {
        if (m_scale == scale) {
            return;
        }
        m_scale = scale;
        UpdateTransform();
    }

    void Entity::SetRotation(const float rads, const glm::vec3& direction) {
        if (rads == 0.0f) {
            return;
        }
        m_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rads), direction);
        UpdateTransform();
    }

    void Entity::Translate(const glm::vec3& delta) {
        if (delta == glm::vec3(0.0f)) {
            return;
        }
        SetPosition(m_position + delta);
    }

    glm::vec3 Entity::GetPosition() {
        return m_position;
    }

    glm::vec3 Entity::GetScale() {
        return m_scale;
    }

    glm::mat4 Entity::GetRotation() {
        return m_rotation;
    }

    glm::mat4 Entity::GetTransformMatrix() {
        return m_transform_martix;
    }
}  // namespace nex
