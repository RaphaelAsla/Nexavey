#include "Input.hpp"

#include "Logger.hpp"
#include "Window.hpp"

namespace nex {
    GLFWwindow* Input::m_window = nullptr;

    void Input::Initialize() {
        m_window = Window::GetGLFWwindow();

        Logger::Log("Input initialization completed successfully, Input::Initialize()", Logger::DEBUG);
    }

    void Input::Tick() {
        if (GetKey(GLFW_KEY_ESCAPE)) {
            Window::Close();
        }
    }

    bool Input::GetKey(int keycode) {
        const int state = glfwGetKey(m_window, keycode);
        return state == GLFW_PRESS;
    }
}  // namespace nex
