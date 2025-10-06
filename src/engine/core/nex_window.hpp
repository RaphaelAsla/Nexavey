#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace nex {
    class NexWindow {
      public:
        NexWindow(const std::string& name, int width, int height);
        ~NexWindow();

        NexWindow(const NexWindow&)            = delete;
        NexWindow& operator=(const NexWindow&) = delete;

        bool shouldClose() const {
            return glfwWindowShouldClose(m_window);
        }

        VkExtent2D getExtent() const {
            return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)};
        }

        bool wasWindowResized() {
            return m_framebuffer_resized;
        }

        void resetWindowResizeFlag() {
            m_framebuffer_resized = false;
        }

        GLFWwindow* getGLFWwindow() const {
            return m_window;
        }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

      private:
        void initWindow();

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

        int  m_width;
        int  m_height;
        bool m_framebuffer_resized = false;

        std::string m_window_name;
        GLFWwindow* m_window;
    };
}  // namespace nex
