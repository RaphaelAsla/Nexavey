#pragma once

#include <stdint.h>

#include "../core/nex_device.hpp"

namespace nex {
    class NexShadowMap {
      public:
        NexShadowMap(NexDevice& deviceRef, uint32_t shadowMapWidth = 1024, uint32_t shadowMapHeight = 1024);
        ~NexShadowMap();

        VkRenderPass getRenderPass() {
            return m_render_pass;
        }

        VkFramebuffer getFrameBuffer() {
            return m_framebuffer;
        }

        uint32_t getWidth() const {
            return m_shadow_map_width;
        }

        uint32_t getHeight() const {
            return m_shadow_map_height;
        }

        VkDescriptorImageInfo getDescriptorInfo() const;

      private:
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();

        NexDevice& m_device;
        uint32_t   m_shadow_map_width;
        uint32_t   m_shadow_map_height;

        VkImage        m_depth_image;
        VkDeviceMemory m_depth_image_memory;
        VkImageView    m_depth_image_view;
        VkSampler      m_shadow_sampler;
        VkRenderPass   m_render_pass;
        VkFramebuffer  m_framebuffer;

        const VkFormat m_depth_format = VK_FORMAT_D16_UNORM;
    };
};  // namespace nex
