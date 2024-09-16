#include "Material.hpp"

#include <cassert>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace nex {
    Material::Material(const std::shared_ptr<Shader>& shader) {
        assert(shader);
        m_shader = shader;
    }

    Material::~Material() {
        m_shader->Delete();
    }

    void Material::Initialize() {}

    void Material::SetTexture(const std::shared_ptr<Texture>& texture) {
        assert(texture);
        m_texture = texture;
    }

    void Material::SetShader(const std::shared_ptr<Shader>& shader) {
        assert(shader);
        m_shader = shader;
    }

    void Material::Use() {
        m_shader->Use();
        m_texture->Bind();
    }

    void Material::SetUniforms(const glm::mat4& transform_matrix) {
        glm::mat4 model = transform_matrix;
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        m_shader->setMat4("model", model);
        m_shader->setMat4("view", view);
        m_shader->setMat4("projection", projection);
    }

    unsigned int Material::GetShaderID() {
        return m_shader->GetID();
    }

    unsigned int Material::GetTextureID() {
        return m_texture->GetID();
    }

    Texture* Material::GetTexture() {
        return m_texture.get();
    }

    Shader* Material::GetShader() {
        return m_shader.get();
    }
}  // namespace nex
