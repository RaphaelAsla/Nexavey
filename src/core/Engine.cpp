#include "Engine.hpp"

#include "../graphics/Renderer.hpp"
#include "Input.hpp"
#include "Logger.hpp"
#include "Window.hpp"
#include "glad/gl.h"

namespace nex {
    Engine::Engine() {
        Window::Initialize();
        Input::Initialize();
        Renderer::Initialize();

        Logger::Log("Engine initialization completed successfully, Engine::Engine()", Logger::DEBUG);
    }

    Engine::~Engine() {
        Window::ShutDown();
        Renderer::ShutDown();

        Logger::Log("Engine destruction completed successfully, Engine::~Engine()", Logger::DEBUG);
    }

    void Engine::Run() {
        while (!Window::ShouldClose()) {
            Window::Tick();
            Input::Tick();
            Renderer::Tick();
        }
    }
}  // namespace nex
