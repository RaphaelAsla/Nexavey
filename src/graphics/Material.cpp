#include "Material.hpp"

#include <cassert>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../core/Window.hpp"

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
        m_shader->setMat4("model", transform_matrix);
        m_shader->setMat4("view", Window::m_active_camera->GetViewMatrix());
        m_shader->setMat4("projection", Window::m_active_camera->GetProjectionMatrix());
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
