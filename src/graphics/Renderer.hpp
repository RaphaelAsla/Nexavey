#pragma once

#include <vector>

#include "../world/Entity.hpp"
#include "Material.hpp"

namespace nex {
    class Renderer {
      public:
        static void Initialize();
        static void ShutDown();
        static void Tick();

        static void CreateEntities();
        static void CreateShadersAndTextures();
        static void Clear();

      private:
        static std::vector<Entity> m_entities;
        static std::vector<std::shared_ptr<Material>> m_materials;
    };
}  // namespace nex
