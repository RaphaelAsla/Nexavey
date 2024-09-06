#pragma once

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
    };
}  // namespace nex
