#include "nex_window.hpp"

#include <stdexcept>

namespace nex {

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    NexWindow::NexWindow(const std::string& name, int width, int height) : m_width(width), m_height(height), m_window_name(name) {
        initWindow();
    }

    NexWindow::~NexWindow() {
        if (m_window) {
            glfwDestroyWindow(m_window);
        }
        glfwTerminate();
    }

    void NexWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = glfwCreateWindow(m_width, m_height, m_window_name.c_str(), nullptr, nullptr);

        glfwSetKeyCallback(m_window, keyCallback);

        if (!m_window) {
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
    }

    void NexWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface");
        }
    }

    void NexWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto nex_window                   = reinterpret_cast<NexWindow*>(glfwGetWindowUserPointer(window));
        nex_window->m_framebuffer_resized = true;
        nex_window->m_width               = width;
        nex_window->m_height              = height;
    }

}  // namespace nex
