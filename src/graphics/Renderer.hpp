#pragma once

#include <vector>

#include "Mesh.hpp"

namespace nex {
    class Renderer {
      public:
        static void Initialize();
        static void ShutDown();
        static void Tick();

        static void CreateShaders();
        static void CreateMeshes();
        static void DrawMeshes();
        static void DeleteMeshes();
        static void Clear();

      private:
        static std::vector<Shader> m_shaders;
        static std::vector<Mesh> m_meshes;
    };
}  // namespace nex
