#include "Renderer.hpp"

#include <glad/gl.h>

#include "../core/Logger.hpp"

namespace nex {
    unsigned int Renderer::shader_program = 0;
    unsigned int Renderer::VAO = 0;
    unsigned int Renderer::VBO = 0;
    std::vector<Shader> Renderer::shaders = {};

    void Renderer::Initialize() {
        CreateShaders();
        MakeTriangle();

        Logger::Log("Renderer initialization completed successfully, Renderer::Initialize()", Logger::DEBUG);
    }

    void Renderer::ShutDown() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shader_program);
        Logger::Log("Renderer shutdown completed successfully, Renderer::ShutDown()", Logger::DEBUG);
    }

    void Renderer::Tick() {
        Clear();
        DrawTriangle();
    }

    void Renderer::CreateShaders() {
        shaders.emplace_back(Shader("../shaders/triangle.vert.glsl", "../shaders/triangle.frag.glsl"));

        Logger::Log("Creation of shaders completed successfully, Renderer::CreateShaders()", Logger::DEBUG);
    }

    void Renderer::MakeTriangle() {
        float vertices[] = {
            // positions         // colors
            0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom left
            0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f   // top
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        Logger::Log("Creation of triangle shape completed successfully, Renderer::MakeTriangle()", Logger::DEBUG);
    }

    void Renderer::DrawTriangle() {
        shaders[0].use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void Renderer::Clear() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}  // namespace nex
