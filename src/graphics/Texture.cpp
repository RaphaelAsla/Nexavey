#include "Texture.hpp"

#include "../core/Logger.hpp"
#include "glad/gl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace nex {
    Texture::Texture(const std::string& tex_path) {
        m_texture_data = stbi_load(tex_path.c_str(), &m_width, &m_height, &m_channels, 0);
        Initialize();
    }

    void Texture::Initialize() {
        glGenTextures(1, &ID);

        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (m_texture_data != nullptr) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_texture_data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            Logger::Log("Failed to load texture", Logger::RELEASE);
        }

        stbi_image_free(m_texture_data);

        Logger::Log("Texture initialization completed successfully, Texture::Initialize()", Logger::DEBUG);
    }

    void Texture::Bind() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    unsigned int Texture::GetID() {
        return ID;
    }
}  // namespace nex
