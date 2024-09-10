#include "Entity.hpp"

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
        m_componenets.emplace_back(component);
    }
}  // namespace nex
