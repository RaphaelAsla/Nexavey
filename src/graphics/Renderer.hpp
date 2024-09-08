#pragma once

#include <vector>

#include "Mesh.hpp"
#include "Shader.hpp"

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
        static std::vector<Shader> shaders;
        static std::vector<Mesh> meshes;
        static unsigned int shader_program;
        static unsigned int VBO, VAO;
    };
}  // namespace nex
