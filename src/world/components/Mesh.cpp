#include "Mesh.hpp"

#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace nex {

    Mesh::Mesh(const std::vector<VertexData>& vertex_data, const std::vector<unsigned int>& indices, const std::shared_ptr<Material>& material)
        : m_vertex_data(vertex_data)
        , m_indices(indices)
        , m_material(material.get()) {}

    Mesh::~Mesh() {
        OnDelete();
    }

    void Mesh::Initialize() {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, m_vertex_data.size() * sizeof(VertexData), m_vertex_data.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(float), m_indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(2 * sizeof(glm::vec3)));
        glEnableVertexAttribArray(2);
    }

    void Mesh::OnCreate() {}

    void Mesh::OnDelete() {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_VAO);
    }

    void Mesh::SetMaterial(const std::shared_ptr<Material>& material) {
        assert(material);
        m_material = material.get();
    }

    void Mesh::SetGeometry(const std::vector<VertexData>& vertex_data, const std::vector<unsigned int>& indices) {
        m_vertex_data = vertex_data;
        m_indices = indices;
        Initialize();
    }

    void Mesh::OnTick() {
        m_material->Use();
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    }

    void Mesh::RotateLeft() {
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, (float)(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));
        unsigned int transformLoc = glGetUniformLocation(m_material->GetShaderID(), "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    }

    void Mesh::RotateRight() {
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, -(float)(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));
        unsigned int transformLoc = glGetUniformLocation(m_material->GetShaderID(), "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    }

}  // namespace nex