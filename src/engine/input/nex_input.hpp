#pragma once

#include <glm/gtc/constants.hpp>

#include "../scene/nex_entity.hpp"

namespace nex {
    class Input {
      public:
        struct KeyMappings {
            int m_move_left     = GLFW_KEY_A;
            int m_move_right    = GLFW_KEY_D;
            int m_move_forward  = GLFW_KEY_W;
            int m_move_backward = GLFW_KEY_S;
            int m_move_up       = GLFW_KEY_E;
            int m_move_down     = GLFW_KEY_Q;
            int m_look_left     = GLFW_KEY_LEFT;
            int m_look_right    = GLFW_KEY_RIGHT;
            int m_look_up       = GLFW_KEY_UP;
            int m_look_down     = GLFW_KEY_DOWN;
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, NexEntity& entity);

        KeyMappings m_keys       = {};
        float       m_move_speed = 3.0f;
        float       m_look_speed = 1.5;
    };
};  // namespace nex
