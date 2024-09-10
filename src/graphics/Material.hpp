#pragma once

#include <memory>

#include "Shader.hpp"
#include "Texture.hpp"

namespace nex {
    class Material {
      public:
        Material() = default;
        ~Material();
        Material(const std::shared_ptr<Shader>& shader);

        void Initialize();
        void SetTexture(const std::shared_ptr<Texture>& texture);
        void SetShader(const std::shared_ptr<Shader>& shader);
        unsigned int GetShaderID();
        unsigned int GetTextureID();
        void Use();

      private:
        std::shared_ptr<Texture> m_texture = nullptr;
        std::shared_ptr<Shader> m_shader = nullptr;
    };
}  // namespace nex
