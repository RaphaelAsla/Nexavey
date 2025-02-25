#pragma once
#include <string>

#include "../world/components/Camera.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace nex {
    class Window {
      public:
        static void        Initialize();
        static void        ShutDown();
        static void        Tick();
        static bool        ShouldClose();
        static void        Close();
        static void        SwapBuffers();
        static GLFWwindow* GetGLFWwindow();

        static int GetWidth() {
            return m_width;
        }

        static int GetHeight() {
            return m_height;
        }

        static std::shared_ptr<Camera> m_active_camera;

      private:
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

        static int         m_width;
        static int         m_height;
        static std::string m_window_title;
        static GLFWwindow* m_window;
    };
}  // namespace nex
