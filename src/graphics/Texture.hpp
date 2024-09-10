#pragma once

#include <iostream>
#include <string>

#include "glad/gl.h"

namespace nex {
    class Texture {
      public:
        Texture() = default;
        ~Texture() = default;
        Texture(const std::string& tex_path);

        void Initialize();
        void Bind();
        unsigned int GetID();

      private:
        unsigned int ID;
        int m_width;
        int m_height;
        int m_channels;
        std::string m_texture_path;
        unsigned char* m_texture_data;
    };
}  // namespace nex
