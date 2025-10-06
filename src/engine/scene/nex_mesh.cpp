#include "nex_mesh.hpp"

#include <cstring>
#include <unordered_map>

#include "../core/nex_utils.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std {
    template <>
    struct hash<nex::NexMesh::Vertex> {
        size_t operator()(const nex::NexMesh::Vertex& vertex) const {
            size_t seed = 0;
            nex::hashCombine(seed, vertex.m_position, vertex.m_color, vertex.m_normal, vertex.m_uv);
            return seed;
        }
    };
};  // namespace std

namespace nex {
    NexMesh::NexMesh(NexDevice& nex_device, const Builder& builder) : m_device(nex_device) {
        createVertexBuffer(builder.m_vertices);
        createIndexBuffer(builder.m_indices);
    }

    NexMesh::~NexMesh() {}

    std::unique_ptr<NexMesh> NexMesh::createModelFromFile(NexDevice& device, const std::string& filepath) {
        Builder builder;
        builder.loadModel(filepath);
        return std::make_unique<NexMesh>(device, builder);
    }

    void NexMesh::createVertexBuffer(const std::vector<Vertex>& vertices) {
        m_vertex_count = static_cast<uint32_t>(vertices.size());
        assert(m_vertex_count >= 3 && "Vertex count must be at least 3");

        VkDeviceSize buffer_size = sizeof(vertices[0]) * m_vertex_count;
        uint32_t     vertex_size = static_cast<uint32_t>(sizeof(vertices[0]));

        NexBuffer staging_buffer(m_device, vertex_size, m_vertex_count, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        staging_buffer.map();
        staging_buffer.writeToBuffer((void*)vertices.data(), buffer_size);

        m_vertex_buffer = std::make_unique<NexBuffer>(m_device, vertex_size, m_vertex_count, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        m_device.copyBuffer(staging_buffer.getBuffer(), m_vertex_buffer->getBuffer(), buffer_size);
    }

    void NexMesh::createIndexBuffer(const std::vector<uint32_t>& indices) {
        m_index_count      = static_cast<uint32_t>(indices.size());
        m_has_index_buffer = m_index_count > 0;

        if (!m_has_index_buffer) {
            return;
        }

        VkDeviceSize buffer_size = sizeof(indices[0]) * m_index_count;
        uint32_t     index_size  = static_cast<uint32_t>(sizeof(indices[0]));

        NexBuffer staging_buffer(m_device, index_size, m_index_count, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        staging_buffer.map();
        staging_buffer.writeToBuffer((void*)indices.data(), buffer_size);

        m_index_buffer = std::make_unique<NexBuffer>(m_device, index_size, m_index_count, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        m_device.copyBuffer(staging_buffer.getBuffer(), m_index_buffer->getBuffer(), buffer_size);
    }

    void NexMesh::bind(VkCommandBuffer command_buffer, uint32_t first_vertex) const {
        VkBuffer     vertex_buffers[] = {m_vertex_buffer->getBuffer()};
        VkDeviceSize offsets[]        = {0};

        vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

        if (m_has_index_buffer) {
            vkCmdBindIndexBuffer(command_buffer, m_index_buffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void NexMesh::draw(VkCommandBuffer command_buffer, uint32_t first_vertex) const {
        if (m_has_index_buffer) {
            vkCmdDrawIndexed(command_buffer, m_index_count, 1, 0, 0, 0);
        } else {
            vkCmdDraw(command_buffer, m_vertex_count, 1, 0, 0);
        }
    }

    std::vector<VkVertexInputBindingDescription> NexMesh::Vertex::getBindingDescriptions() {
        return {{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
    }

    std::vector<VkVertexInputAttributeDescription> NexMesh::Vertex::getAttributesDescriptions() {
        return {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, m_position)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, m_color)},
            {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, m_normal)},
            {3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, m_uv)},
        };
    }

    void NexMesh::Builder::loadModel(const std::string& filepath) {
        tinyobj::attrib_t                attrib;
        std::vector<tinyobj::shape_t>    shapes;
        std::vector<tinyobj::material_t> materials;

        std::string warn, err;
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
            throw std::runtime_error("Failed to load model: " + warn + err);
        }

        m_vertices.clear();
        m_indices.clear();

        std::unordered_map<Vertex, uint32_t> unique_vertices = {};

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex = {};

                if (index.vertex_index >= 0) {
                    vertex.m_position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2],
                    };

                    vertex.m_color = {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2],
                    };
                }

                if (index.normal_index >= 0) {
                    vertex.m_normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                    };
                }

                if (index.texcoord_index >= 0) {
                    vertex.m_uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1],  // we flipped it, to match Vulkan's coordinate system
                    };
                }

                if (unique_vertices.count(vertex) == 0) {
                    unique_vertices[vertex] = static_cast<uint32_t>(m_vertices.size());
                    m_vertices.push_back(vertex);
                }
                m_indices.push_back(unique_vertices[vertex]);
            }
        }
    }

};  // namespace nex
