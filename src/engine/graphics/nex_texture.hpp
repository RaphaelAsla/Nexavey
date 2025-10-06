#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <string>

#include "../core/nex_device.hpp"

namespace nex {
    class NexTexture {
      public:
        NexTexture(NexDevice& device, const std::string& filepath);
        ~NexTexture();

        NexTexture(const NexTexture&)            = delete;
        NexTexture& operator=(const NexTexture&) = delete;

        VkDescriptorImageInfo getDescriptorInfo() const {
            return m_descriptor;
        }

        void updateDescriptor();
        void transitionLayout(VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

        static std::shared_ptr<NexTexture> createTextureFromFile(NexDevice& device, const std::string& filepath) {
            return std::make_shared<NexTexture>(device, filepath);
        }

      private:
        void createTextureImage(const std::string& filepath);
        void createTextureImageView();
        void createTextureSampler();

        VkDescriptorImageInfo m_descriptor = {};

        NexDevice& m_device;

        VkImage        m_texture_image        = nullptr;
        VkDeviceMemory m_texture_image_memory = nullptr;
        VkImageView    m_texture_image_view   = nullptr;
        VkSampler      m_texture_sampler      = nullptr;
        VkFormat       m_texture_format;
        VkImageLayout  m_texture_image_layout;

        uint32_t m_mipmap_levels = 1;
        uint32_t m_layer_count   = 1;
    };
}  // namespace nex
