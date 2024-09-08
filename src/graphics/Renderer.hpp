#pragma once

#include <vector>

#include "Shader.hpp"

namespace nex {
    class Renderer {
      public:
        static void Initialize();
        static void ShutDown();
        static void Tick();

        static void CreateShaders();
        static void MakeTriangle();
        static void DrawTriangle();
        static void Clear();

      private:
        static unsigned int shader_program;
        static unsigned int VBO, VAO;
        static std::vector<Shader> shaders;
    };
}  // namespace nex
