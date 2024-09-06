#include "Renderer.h"

#include <glad/gl.h>

#include "../core/Logger.h"

namespace nex {
    unsigned int Renderer::shader_program;
    unsigned int Renderer::VAO;
    unsigned int Renderer::VBO;

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
        const char* vertex_shader_source =
            "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
            "}\0";

        const char* fragment_shader_source =
            "#version 330 core\n"
            "out vec4 FragColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
            "}\0";

        unsigned int vertex_shader;
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
        glCompileShader(vertex_shader);

        unsigned int fragment_shader;
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
        glCompileShader(fragment_shader);

        shader_program = glCreateProgram();

        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        Logger::Log("Creation of shaders completed successfully, Renderer::CreateShaders()", Logger::DEBUG);
    }

    void Renderer::MakeTriangle() {
        float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        Logger::Log("Creation of triangle shape completed successfully, Renderer::MakeTriangle()", Logger::DEBUG);
    }

    void Renderer::DrawTriangle() {
        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void Renderer::Clear() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}  // namespace nex
