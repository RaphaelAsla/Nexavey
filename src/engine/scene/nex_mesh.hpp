#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <memory>

#include "../graphics/nex_buffer.hpp"
#include "../core/nex_device.hpp"

namespace nex {
    class NexMesh {
      public:
        struct Vertex {
            glm::vec3 m_position = {};
            glm::vec3 m_color    = {};
            glm::vec3 m_normal   = {};
            glm::vec2 m_uv       = {};

            static std::vector<VkVertexInputBindingDescription>   getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributesDescriptions();

            bool operator==(const Vertex& other) const {
                return m_position == other.m_position && m_color == other.m_color && m_normal == other.m_normal && m_uv == other.m_uv;
            }
        };

        struct Builder {
            std::vector<Vertex>   m_vertices = {};
            std::vector<uint32_t> m_indices  = {};

            void loadModel(const std::string& filepath);
        };

        NexMesh(NexDevice& nex_device, const Builder& builder);
        ~NexMesh();

        NexMesh(const NexMesh&)            = delete;
        NexMesh& operator=(const NexMesh&) = delete;

        static std::unique_ptr<NexMesh> createModelFromFile(NexDevice& device, const std::string& filepath);

        void bind(VkCommandBuffer command_buffer, uint32_t first_vertex) const;
        void draw(VkCommandBuffer command_buffer, uint32_t first_vertex) const;

      private:
        void createVertexBuffer(const std::vector<Vertex>& vertices);
        void createIndexBuffer(const std::vector<uint32_t>& indices);

        NexDevice& m_device;

        std::unique_ptr<NexBuffer> m_vertex_buffer;
        uint32_t                   m_vertex_count;

        bool                       m_has_index_buffer = false;
        std::unique_ptr<NexBuffer> m_index_buffer;
        uint32_t                   m_index_count;
    };
};  // namespace nex
