#include "nex_shadowmap.hpp"

#include <array>
#include <stdexcept>
namespace nex {
    NexShadowMap::NexShadowMap(NexDevice& deviceRef, uint32_t shadowMapWidth, uint32_t shadowMapHeight)
        : m_device{deviceRef}
        , m_shadow_map_width{shadowMapWidth}
        , m_shadow_map_height{shadowMapHeight} {
        createDepthResources();
        createRenderPass();
        createFramebuffers();
    }

    NexShadowMap::~NexShadowMap() {
        vkDestroySampler(m_device.device(), m_shadow_sampler, nullptr);
        vkDestroyImageView(m_device.device(), m_depth_image_view, nullptr);
        vkDestroyImage(m_device.device(), m_depth_image, nullptr);
        vkFreeMemory(m_device.device(), m_depth_image_memory, nullptr);
        vkDestroyFramebuffer(m_device.device(), m_framebuffer, nullptr);
        vkDestroyRenderPass(m_device.device(), m_render_pass, nullptr);
    }

    void NexShadowMap::createDepthResources() {
        VkImageCreateInfo image_info{};
        image_info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType     = VK_IMAGE_TYPE_2D;
        image_info.extent.width  = m_shadow_map_width;
        image_info.extent.height = m_shadow_map_height;
        image_info.extent.depth  = 1;
        image_info.mipLevels     = 1;
        image_info.arrayLayers   = 1;
        image_info.samples       = VK_SAMPLE_COUNT_1_BIT;
        image_info.tiling        = VK_IMAGE_TILING_OPTIMAL;
        image_info.format        = m_depth_format;
        image_info.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;  // We will sample directly from the depth attachment
        image_info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

        m_device.createImageWithInfo(image_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depth_image, m_depth_image_memory);

        VkImageViewCreateInfo view_info{};
        view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format                          = m_depth_format;
        view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
        view_info.subresourceRange.baseMipLevel   = 0;
        view_info.subresourceRange.levelCount     = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount     = 1;
        view_info.image                           = m_depth_image;

        if (vkCreateImageView(m_device.device(), &view_info, nullptr, &m_depth_image_view) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }

        VkSamplerCreateInfo sampler_info{};
        sampler_info.sType         = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter     = VK_FILTER_LINEAR;
        sampler_info.minFilter     = VK_FILTER_LINEAR;
        sampler_info.mipmapMode    = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.addressModeU  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        sampler_info.addressModeV  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        sampler_info.addressModeW  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        sampler_info.mipLodBias    = 0.0f;
        sampler_info.maxAnisotropy = 1.0;
        sampler_info.minLod        = 0.0f;
        sampler_info.maxLod        = 1.0f;
        sampler_info.borderColor   = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp     = VK_COMPARE_OP_NEVER;

        if (vkCreateSampler(m_device.device(), &sampler_info, nullptr, &m_shadow_sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void NexShadowMap::createRenderPass() {
        VkAttachmentDescription depth_attachment{};
        depth_attachment.format         = m_depth_format;
        depth_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        depth_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;   // Create depth at start of render pass
        depth_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;  // We will later sample from the depth attachment, so we want to store the depth data
        depth_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;                        // We don't care about initial layout
        depth_attachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;  // For sampling later

        VkAttachmentReference depth_attachment_ref{};
        depth_attachment_ref.attachment = 0;
        depth_attachment_ref.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;  // During the subpass, we will be using the depth attachment as a depth/stencil attachment

        VkSubpassDescription subpass    = {};
        subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount    = 0;
        subpass.pDepthStencilAttachment = &depth_attachment_ref;

        std::array<VkSubpassDependency, 2> dependencies{};

        dependencies[0].srcSubpass      = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass      = 0;
        dependencies[0].srcStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[0].srcAccessMask   = VK_ACCESS_SHADER_READ_BIT;
        dependencies[0].dstAccessMask   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass      = 0;
        dependencies[1].dstSubpass      = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask    = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependencies[1].dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[1].srcAccessMask   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments    = &depth_attachment;
        render_pass_info.subpassCount    = 1;
        render_pass_info.pSubpasses      = &subpass;
        render_pass_info.dependencyCount = static_cast<uint32_t>(dependencies.size());
        render_pass_info.pDependencies   = dependencies.data();

        if (vkCreateRenderPass(m_device.device(), &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void NexShadowMap::createFramebuffers() {
        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass      = m_render_pass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments    = &m_depth_image_view;
        framebuffer_info.width           = m_shadow_map_width;
        framebuffer_info.height          = m_shadow_map_height;
        framebuffer_info.layers          = 1;

        if (vkCreateFramebuffer(m_device.device(), &framebuffer_info, nullptr, &m_framebuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    VkDescriptorImageInfo NexShadowMap::getDescriptorInfo() const {
        VkDescriptorImageInfo image_info{};
        image_info.sampler     = m_shadow_sampler;
        image_info.imageView   = m_depth_image_view;
        image_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        return image_info;
    }
};  // namespace nex
