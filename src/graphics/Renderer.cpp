#include "Renderer.hpp"

#include <glad/gl.h>

#include "../core/Logger.hpp"

namespace nex {
    unsigned int Renderer::shader_program = 0;
    unsigned int Renderer::VAO = 0;
    unsigned int Renderer::VBO = 0;
    std::vector<Shader> Renderer::shaders = {};
    std::vector<Mesh> Renderer::meshes = {};

    void Renderer::Initialize() {
        CreateShaders();
        CreateMeshes();
        Logger::Log("Renderer initialization completed successfully, Renderer::Initialize()", Logger::DEBUG);
    }

    void Renderer::ShutDown() {
        DeleteMeshes();
        for (Shader& shader : shaders) {
            shader.Delete();
        }
        Logger::Log("Renderer shutdown completed successfully, Renderer::ShutDown()", Logger::DEBUG);
    }

    void Renderer::Tick() {
        Clear();
        DrawMeshes();
    }

    void Renderer::CreateShaders() {
        shaders.emplace_back(Shader("../shaders/simple.vert.glsl", "../shaders/simple.frag.glsl"));

        Logger::Log("Creation of shaders completed successfully, Renderer::CreateShaders()", Logger::DEBUG);
    }

    void Renderer::CreateMeshes() {
        // rectangle
        std::vector<VertexData> square_vertices;
        square_vertices.emplace_back(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        square_vertices.emplace_back(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        square_vertices.emplace_back(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        square_vertices.emplace_back(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        std::vector<unsigned int> square_indices = {0, 1, 3, 1, 2, 3};
        meshes.emplace_back(square_vertices, square_indices);

        // triangle
        std::vector<VertexData> triangle_vertices;
        triangle_vertices.emplace_back(glm::vec3(-0.25f, -0.25f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        triangle_vertices.emplace_back(glm::vec3(0.25f, -0.25f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        triangle_vertices.emplace_back(glm::vec3(0.0f, 0.25f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        std::vector<unsigned int> triangle_indices = {0, 1, 2};
        meshes.emplace_back(triangle_vertices, triangle_indices);

        Logger::Log("Creation of meshes completed successfully, Renderer::CreateShaders()", Logger::DEBUG);
    }

    void Renderer::DrawMeshes() {
        shaders[0].Use();

        for (Mesh& mesh : meshes) {
            mesh.DrawMesh();
        }
    }

    void Renderer::DeleteMeshes() {
        for (Mesh& mesh : meshes) {
            mesh.DeleteMesh();
        }

        Logger::Log("Deletion of meshes completed successfully, Renderer::CreateShaders()", Logger::DEBUG);
    }

    void Renderer::Clear() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}  // namespace nex
