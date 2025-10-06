#pragma once

#include <vulkan/vulkan.h>

#include "nex_camera.hpp"
#include "nex_entity.hpp"

namespace nex {
    class NexDescriptorPool;

#define MAX_LIGHTS 10

    struct PointLight {
        glm::vec4 m_position = {};  // ignore w (used for alignment)
        glm::vec4 m_color    = {};  // w is intensity
    };

    struct GlobalUbo {
        glm::mat4  m_projection_matrix   = {1.0f};
        glm::mat4  m_view_matrix         = {1.0f};
        glm::mat4  m_inverse_view_matrix = {1.0f};
        glm::vec4  m_ambient_color       = {1.0f, 1.0f, 1.0f, 0.2f};
        PointLight m_point_lights[MAX_LIGHTS];
        int        m_light_count = 0;
    };

    struct NexFrameInfo {
        int                m_frame_index;
        float              m_frame_time;
        VkCommandBuffer    m_command_buffer;
        NexCamera&         m_camera;
        VkDescriptorSet    m_global_descriptor_set;
        NexDescriptorPool& m_frame_descriptor_pool;
        NexEntity::Map&    m_entities;
    };

}  // namespace nex
