#include "Window.h"

#include "Logger.h"
#include "glad/gl.h"

namespace nex {
    int Window::m_width = 1280;
    int Window::m_height = 720;
    std::string Window::m_window_title = "Nexavey";
    GLFWwindow* Window::m_window = nullptr;

    void Window::Initialize() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        m_window = glfwCreateWindow(m_width, m_height, m_window_title.c_str(), NULL, NULL);

        glfwMakeContextCurrent(m_window);

        if (gladLoadGL(glfwGetProcAddress) == 0) {
            Logger::Log("Failed to initialize GLAD, Window::Initialize()", Logger::RELEASE);
        }

        glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);

        Logger::Log("Window initialization completed successfully, Window::Initialize()", Logger::DEBUG);
    }

    void Window::ShutDown() {
        glfwDestroyWindow(m_window);
        glfwTerminate();

        Logger::Log("Window shutdown completed successfully, Window::ShutDown()", Logger::DEBUG);
    }

    void Window::Tick() {
        Window::SwapBuffers();
        glfwPollEvents();
    }

    bool Window::ShouldClose() {
        return glfwWindowShouldClose(m_window);
    }

    void Window::Close() {
        glfwSetWindowShouldClose(m_window, true);
    }

    void Window::SwapBuffers() {
        glfwSwapBuffers(m_window);
    }

    GLFWwindow* Window::GetGLFWwindow() {
        return m_window;
    }

    void Window::FramebufferSizeCallback([[maybe_unused]] GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }
}  // namespace nex
