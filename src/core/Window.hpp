#pragma once
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace nex {
    class Window {
      public:
        static void Initialize();
        static void ShutDown();
        static void Tick();
        static bool ShouldClose();
        static void Close();
        static void SwapBuffers();
        static GLFWwindow* GetGLFWwindow();

      private:
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

        static int m_width;
        static int m_height;
        static std::string m_window_title;
        static GLFWwindow* m_window;
    };
}  // namespace nex
