#include "Renderer.hpp"

#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "../core/Logger.hpp"
#include "../core/Window.hpp"
#include "../world/components/Camera.hpp"
#include "../world/components/Mesh.hpp"

namespace nex {
    std::vector<std::shared_ptr<Entity>>   Renderer::m_entities  = {};
    std::vector<std::shared_ptr<Material>> Renderer::m_materials = {};

    void Renderer::Initialize() {
        CreateShadersAndTextures();
        CreateEntities();
        glEnable(GL_DEPTH_TEST);
        Logger::Log("Renderer initialization completed successfully, Renderer::Initialize()", Logger::DEBUG);
    }

    void Renderer::ShutDown() {
        for (auto& entity : m_entities) {
            entity->OnDelete();
        }
        Logger::Log("Renderer shutdown completed successfully, Renderer::ShutDown()", Logger::DEBUG);
    }

    void Renderer::Tick() {
        m_entities[0]->SetRotation((float)glfwGetTime() * 100, glm::vec3(1.0f, 1.0f, 0.0f));
        m_entities[1]->SetRotation(-(float)glfwGetTime() * 100, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

        // glm::vec3 center = glm::vec3(0.0f, 0.0f, -5.0f);

        // float R      = 2.0f;
        // float radius = 1.0f;

        // float angle1 = glfwGetTime() * 4.0f;
        // float angle2 = angle1 + glm::pi<float>() / 2.0f;

        // float x1 = (R + radius * cos(angle1)) * cos(angle1 / 10.0f);
        // float y1 = sin(angle1);
        // float z1 = (R + radius * cos(angle1)) * sin(angle1 / 10.0f);

        // float x2 = (R + radius * cos(angle2)) * cos(angle2 / 10.0f);
        // float y2 = sin(angle2);
        // float z2 = (R + radius * cos(angle2)) * sin(angle1 / 10.0f);

        // m_entities[0]->SetPosition(center + glm::vec3(x1, y1, z1));
        // m_entities[1]->SetPosition(center + glm::vec3(x2, y2, z2));

        m_entities[0]->SetPosition(center + glm::vec3(1.0, 0.0, 0.0));
        m_entities[1]->SetPosition(center + glm::vec3(-1.0, 0.0, 0.0));
        m_entities[0]->SetScale(glm::vec3(0.4f));
        m_entities[1]->SetScale(glm::vec3(0.4f));

        Clear();

        for (size_t i = 0; i < m_entities.size(); i++) {
            m_entities[i]->Tick();
        }
    }

    void Renderer::CreateShadersAndTextures() {
        auto mat = std::make_shared<Material>(std::make_shared<Shader>("../shaders/simple.vert.glsl", "../shaders/simple.frag.glsl"));
        mat->SetTexture(std::make_shared<Texture>("../assets/textures/wall.jpg"));
        m_materials.emplace_back(mat);
    }

    void Renderer::CreateEntities() {
        std::shared_ptr<Entity> rectangle = std::make_shared<Entity>();
        std::shared_ptr<Entity> triangle  = std::make_shared<Entity>();
        std::shared_ptr<Entity> camera    = std::make_shared<Entity>();

        std::vector<VertexData> cube_vertices = {
            // Front face
            {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},    // Top-right
            {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},   // Bottom-right
            {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},  // Bottom-left
            {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},   // Top-left

            // Back face
            {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},

            // Left face
            {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},

            // Right face
            {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},

            // Top face
            {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},

            // Bottom face
            {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
        };

        std::vector<unsigned int> cube_indices = {// Front face
                                                  0, 1, 2, 0, 2, 3,
                                                  // Back face
                                                  4, 5, 6, 4, 6, 7,
                                                  // Left face
                                                  8, 9, 10, 8, 10, 11,
                                                  // Right face
                                                  12, 13, 14, 12, 14, 15,
                                                  // Top face
                                                  16, 17, 18, 16, 18, 19,
                                                  // Bottom face
                                                  20, 21, 22, 20, 22, 23};

        std::vector<VertexData> pyramid_vertices = {
            // Base face
            {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},    // Bottom-right
            {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},   // Bottom-left
            {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},  // Top-left
            {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},   // Top-right

            // Apex vertex (used for all sides)
            {glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.5f, 0.5f)},  // Apex

            // Front face
            {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},   // Base bottom-right
            {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},  // Base bottom-left
            {glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 0.5f)},    // Apex

            // Right face
            {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},   // Base bottom-left
            {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},  // Base top-left
            {glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.5f, 0.5f)},     // Apex

            // Back face
            {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},  // Base top-left
            {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},   // Base top-right
            {glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.5f, 0.5f)},     // Apex

            // Left face
            {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},  // Base top-right
            {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},   // Base bottom-right
            {glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.5f, 0.5f)},    // Apex
        };

        std::vector<unsigned int> pyramid_indices = {// Base face
                                                     0, 1, 2, 0, 2, 3,

                                                     // Front face
                                                     4, 5, 6,

                                                     // Right face
                                                     7, 8, 9,

                                                     // Back face
                                                     10, 11, 12,

                                                     // Left face
                                                     13, 14, 15};

        // Create a cube mesh
        auto cube_mesh = std::make_shared<Mesh>();
        cube_mesh->SetMaterial(m_materials[0]);
        cube_mesh->SetGeometry(cube_vertices, cube_indices);

        // Create a pyramid mesh
        auto pyramid_mesh = std::make_shared<Mesh>();
        pyramid_mesh->SetMaterial(m_materials[0]);
        pyramid_mesh->SetGeometry(pyramid_vertices, pyramid_indices);

        rectangle->AddComponent(cube_mesh);
        triangle->AddComponent(pyramid_mesh);

        // Camera
        auto active_camera = std::make_shared<Camera>();
        camera->AddComponent(active_camera);
        camera->SetPosition(glm::vec3(0.0f, 8.0f, 0.0f));
        camera->Initialize();
        Window::m_active_camera = active_camera;

        m_entities.emplace_back(rectangle);
        m_entities.emplace_back(triangle);
        m_entities.emplace_back(camera);

        Logger::Log("Creation of Entities completed successfully, Renderer::CreateEntities()", Logger::DEBUG);
    }

    void Renderer::Clear() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}  // namespace nex
