#include "Renderer.hpp"

#include <glad/gl.h>

#include "../core/Logger.hpp"

namespace nex {
    std::vector<Shader> Renderer::m_shaders = {};
    std::vector<Mesh> Renderer::m_meshes = {};

    void Renderer::Initialize() {
        CreateShaders();
        CreateMeshes();
        Logger::Log("Renderer initialization completed successfully, Renderer::Initialize()", Logger::DEBUG);
    }

    void Renderer::ShutDown() {
        DeleteMeshes();
        for (Shader& shader : m_shaders) {
            shader.Delete();
        }
        Logger::Log("Renderer shutdown completed successfully, Renderer::ShutDown()", Logger::DEBUG);
    }

    void Renderer::Tick() {
        Clear();
        DrawMeshes();
    }

    void Renderer::CreateShaders() {
        m_shaders.emplace_back(Shader("../shaders/simple.vert.glsl", "../shaders/simple.frag.glsl"));

        Logger::Log("Creation of shaders completed successfully, Renderer::CreateShaders()", Logger::DEBUG);
    }

    void Renderer::CreateMeshes() {
        // rectangle
        std::vector<VertexData> square_vertices;
        square_vertices.emplace_back(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        square_vertices.emplace_back(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        square_vertices.emplace_back(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        square_vertices.emplace_back(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
        std::vector<unsigned int> square_indices = {0, 1, 3, 1, 2, 3};
        Texture tex("../assets/textures/wall.jpg");
        m_meshes.emplace_back(square_vertices, square_indices, tex, m_shaders[0]);

        // triangle
        std::vector<VertexData> triangle_vertices;
        triangle_vertices.emplace_back(glm::vec3(-0.25f, -0.25f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        triangle_vertices.emplace_back(glm::vec3(0.25f, -0.25f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        triangle_vertices.emplace_back(glm::vec3(0.0f, 0.25f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f));
        std::vector<unsigned int> triangle_indices = {0, 1, 2};
        m_meshes.emplace_back(triangle_vertices, triangle_indices, tex, m_shaders[0]);

        Logger::Log("Creation of meshes completed successfully, Renderer::CreateShaders()", Logger::DEBUG);
    }

    void Renderer::DrawMeshes() {
        m_shaders[0].Use();

        for (size_t i = 0; i < m_meshes.size(); i++) {
            m_meshes[i].DrawMesh();
            switch (i) {
                case 0:
                    m_meshes[i].RotateLeft();
                    break;
                case 1:
                    m_meshes[i].RotateRight();
                    break;
            }
        }
    }

    void Renderer::DeleteMeshes() {
        for (Mesh& mesh : m_meshes) {
            mesh.DeleteMesh();
        }

        Logger::Log("Deletion of meshes completed successfully, Renderer::CreateShaders()", Logger::DEBUG);
    }

    void Renderer::Clear() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}  // namespace nex
