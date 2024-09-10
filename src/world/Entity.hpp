#pragma once

#include <memory>
#include <vector>

#include "components/Component.hpp"

namespace nex {
    class Entity {
      public:
        Entity() = default;
        ~Entity() = default;

        void Initialize();
        void OnDelete();
        void Tick();
        void AddComponent(const std::shared_ptr<Component>& component);

      private:
        std::vector<std::shared_ptr<Component>> m_componenets;
    };
}  // namespace nex
