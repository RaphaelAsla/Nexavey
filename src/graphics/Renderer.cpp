#include "Renderer.hpp"

#include <glad/gl.h>

#include "../core/Logger.hpp"
#include "../world/components/Mesh.hpp"

namespace nex {
    std::vector<Entity> Renderer::m_entities = {};
    std::vector<std::shared_ptr<Material>> Renderer::m_materials = {};

    void Renderer::Initialize() {
        CreateShadersAndTextures();
        CreateEntities();
        Logger::Log("Renderer initialization completed successfully, Renderer::Initialize()", Logger::DEBUG);
    }

    void Renderer::ShutDown() {
        for (Entity& entity : m_entities) {
            entity.OnDelete();
        }
        Logger::Log("Renderer shutdown completed successfully, Renderer::ShutDown()", Logger::DEBUG);
    }

    void Renderer::Tick() {
        Clear();
        for (size_t i = 0; i < m_entities.size(); i++) {
            m_entities[i].Tick();
        }
    }

    void Renderer::CreateShadersAndTextures() {
        auto mat = std::make_shared<Material>(std::make_shared<Shader>("../shaders/simple.vert.glsl", "../shaders/simple.frag.glsl"));
        mat->SetTexture(std::make_shared<Texture>("../assets/textures/trippy.jpg"));
        m_materials.emplace_back(mat);
    }

    void Renderer::CreateEntities() {
        Entity rectangle;
        Entity triangle;

        // rectangle Mesh
        std::vector<VertexData> square_vertices;
        std::vector<unsigned int> square_indices = {0, 1, 3, 1, 2, 3};
        square_vertices.emplace_back(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        square_vertices.emplace_back(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        square_vertices.emplace_back(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        square_vertices.emplace_back(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
        auto rectangle_mesh = std::make_shared<Mesh>();
        rectangle_mesh->SetMaterial(m_materials[0]);
        rectangle_mesh->SetGeometry(square_vertices, square_indices);

        // triangle
        std::vector<VertexData> triangle_vertices;
        std::vector<unsigned int> triangle_indices = {0, 1, 2};
        triangle_vertices.emplace_back(glm::vec3(-0.25f, -0.25f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        triangle_vertices.emplace_back(glm::vec3(0.25f, -0.25f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        triangle_vertices.emplace_back(glm::vec3(0.0f, 0.25f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f));
        auto triangle_mesh = std::make_shared<Mesh>();
        triangle_mesh->SetMaterial(m_materials[0]);
        triangle_mesh->SetGeometry(triangle_vertices, triangle_indices);

        rectangle.AddComponent(rectangle_mesh);
        triangle.AddComponent(triangle_mesh);

        m_entities.emplace_back(rectangle);
        m_entities.emplace_back(triangle);

        Logger::Log("Creation of Entities completed successfully, Renderer::CreateEntities()", Logger::DEBUG);
    }

    void Renderer::Clear() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}  // namespace nex
