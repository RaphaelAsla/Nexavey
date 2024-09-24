#pragma once

#include <glm/glm.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace nex {
    class Input {
      public:
        static void Initialize();
        static void Tick();
        static bool GetKey(int keycode);
        static glm::vec2 GetMousePos();

      private:
        static GLFWwindow* m_window;
    };
}  // namespace nex
