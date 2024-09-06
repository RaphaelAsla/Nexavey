#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace nex {
    class Input {
      public:
        static void Initialize();
        static void Tick();
        static bool GetKey(int keycode);

      private:
        static GLFWwindow* m_window;
    };
}  // namespace nex
