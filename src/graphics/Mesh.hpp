#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

#include "Shader.hpp"
#include "Texture.hpp"

namespace nex {
    struct VertexData {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 uv;
    };

    class Mesh {
      public:
        Mesh() = default;
        ~Mesh() = default;

        Mesh(const std::vector<VertexData>& vert_data, const std::vector<unsigned int>& indices, const Texture& tex, const Shader& shdr);

        void CreateMesh();
        void DeleteMesh();
        void DrawMesh();
        void RotateLeft();
        void RotateRight();

      private:
        std::vector<VertexData> m_vertex_data;
        std::vector<unsigned int> m_indices;
        Texture m_texture;
        unsigned int m_VAO;
        unsigned int m_VBO;
        unsigned int m_EBO;
        Shader m_shader;
    };
}  // namespace nex
