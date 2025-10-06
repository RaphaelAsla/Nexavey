#include "nex_texture.hpp"

#include <stdexcept>

#include "nex_buffer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

namespace nex {
    NexTexture::NexTexture(NexDevice& device, const std::string& filepath) : m_device(device) {
        createTextureImage(filepath);
        createTextureImageView();
        createTextureSampler();
    }

    NexTexture::~NexTexture() {
        vkDestroySampler(m_device.device(), m_texture_sampler, nullptr);
        vkDestroyImageView(m_device.device(), m_texture_image_view, nullptr);
        vkDestroyImage(m_device.device(), m_texture_image, nullptr);
        vkFreeMemory(m_device.device(), m_texture_image_memory, nullptr);
    }

    void NexTexture::updateDescriptor() {
        m_descriptor.sampler     = m_texture_sampler;
        m_descriptor.imageView   = m_texture_image_view;
        m_descriptor.imageLayout = m_texture_image_layout;
    }

    void NexTexture::transitionLayout(VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout) {
        VkImageMemoryBarrier barrier = {};
        barrier.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout            = oldLayout;
        barrier.newLayout            = newLayout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image                           = m_texture_image;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = m_mipmap_levels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = m_layer_count;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (m_texture_format == VK_FORMAT_D32_SFLOAT_S8_UINT || m_texture_format == VK_FORMAT_D24_UNORM_S8_UINT) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        VkPipelineStageFlags source_stage;
        VkPipelineStageFlags destination_stage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            source_stage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            source_stage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            source_stage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            source_stage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            // This says that any cmd that acts in color output or after (dstStage)
            // that needs read or write access to a resource
            // must wait until all previous read accesses in fragment shader
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

            source_stage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            destination_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(commandBuffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    void NexTexture::createTextureImage(const std::string& filepath) {
        int          width, height, channels;
        stbi_uc*     pixels     = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        VkDeviceSize image_size = width * height * 4;

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        // m_mipmap_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
        m_mipmap_levels = 1;

        NexBuffer staging_buffer(m_device, image_size, 1, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        staging_buffer.map();
        staging_buffer.writeToBuffer(pixels, width * height * 4);

        stbi_image_free(pixels);

        m_texture_format = VK_FORMAT_R8G8B8A8_SRGB;

        VkImageCreateInfo image_info = {};
        image_info.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType         = VK_IMAGE_TYPE_2D;
        image_info.extent.width      = width;
        image_info.extent.height     = height;
        image_info.extent.depth      = 1;
        image_info.mipLevels         = m_mipmap_levels;
        image_info.arrayLayers       = 1;
        image_info.format            = VK_FORMAT_R8G8B8A8_SRGB;
        image_info.tiling            = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage             = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_info.samples           = VK_SAMPLE_COUNT_1_BIT;
        image_info.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;

        m_device.createImageWithInfo(image_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_texture_image, m_texture_image_memory);
        m_device.transitionImageLayout(m_texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        m_device.copyBufferToImage(staging_buffer.getBuffer(), m_texture_image, static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1);
        m_device.transitionImageLayout(m_texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_mipmap_levels, m_layer_count);

        m_texture_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    void NexTexture::createTextureImageView() {
        VkImageViewCreateInfo view_info           = {};
        view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image                           = m_texture_image;
        view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format                          = VK_FORMAT_R8G8B8A8_SRGB;
        view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel   = 0;
        view_info.subresourceRange.levelCount     = m_mipmap_levels;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount     = m_layer_count;

        if (vkCreateImageView(m_device.device(), &view_info, nullptr, &m_texture_image_view) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }

    void NexTexture::createTextureSampler() {
        VkSamplerCreateInfo sampler_info     = {};
        sampler_info.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter               = VK_FILTER_LINEAR;
        sampler_info.minFilter               = VK_FILTER_LINEAR;
        sampler_info.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable        = VK_TRUE;
        sampler_info.maxAnisotropy           = 16.0f;
        sampler_info.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;

        // these fields useful for percentage close filtering for shadow maps
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp     = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode    = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.mipLodBias    = 0.0f;
        sampler_info.minLod        = 0.0f;
        sampler_info.maxLod        = static_cast<float>(m_mipmap_levels);

        if (vkCreateSampler(m_device.device(), &sampler_info, nullptr, &m_texture_sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

};  // namespace nex
