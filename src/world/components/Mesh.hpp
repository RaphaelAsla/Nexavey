#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

#include "../../graphics/Material.hpp"
#include "Component.hpp"

namespace nex {
    struct VertexData {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 uv;
    };

    class Mesh : public Component {
      public:
        Mesh() = default;
        Mesh(const std::vector<VertexData>& vertex_data, const std::vector<unsigned int>& indices, const std::shared_ptr<Material>& material);
        ~Mesh();

        void Initialize();
        void OnCreate();
        void OnDelete();
        void OnTick();

        void SetMaterial(const std::shared_ptr<Material>& material);
        void SetGeometry(const std::vector<VertexData>& vertex_data, const std::vector<unsigned int>& indices);
        void RotateLeft();
        void RotateRight();

      private:
        std::vector<VertexData> m_vertex_data;
        std::vector<unsigned int> m_indices;
        Material* m_material = nullptr;
        unsigned int m_VAO;
        unsigned int m_VBO;
        unsigned int m_EBO;
    };
}  // namespace nex
