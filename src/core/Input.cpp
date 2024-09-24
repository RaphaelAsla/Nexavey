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
        glfwPollEvents();
        if (GetKey(GLFW_KEY_ESCAPE)) {
            Window::Close();
        }
    }

    bool Input::GetKey(int keycode) {
        const int state = glfwGetKey(m_window, keycode);
        return state == GLFW_PRESS;
    }

    glm::vec2 Input::GetMousePos() {
        double mouse_x;
        double mouse_y;

        glfwGetCursorPos(Window::GetGLFWwindow(), &mouse_x, &mouse_y);

        return glm::vec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
    }
}  // namespace nex
