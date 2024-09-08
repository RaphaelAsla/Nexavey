#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

namespace nex {
    struct VertexData {
        glm::vec3 position;
        glm::vec3 color;
    };

    class Mesh {
      public:
        Mesh() = default;
        ~Mesh();

        Mesh(const std::vector<VertexData>& vert_data, const std::vector<unsigned int>& indices);

        void CreateMesh();
        void DeleteMesh();
        void DrawMesh();

      private:
        std::vector<VertexData> vertex_data;
        std::vector<unsigned int> indices;

        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
    };
}  // namespace nex
