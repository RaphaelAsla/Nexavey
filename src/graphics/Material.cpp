#include "Material.hpp"

#include <cassert>

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

    unsigned int Material::GetShaderID() {
        return m_shader->GetID();
    }

    unsigned int Material::GetTextureID() {
        return m_texture->GetID();
    }
}  // namespace nex
