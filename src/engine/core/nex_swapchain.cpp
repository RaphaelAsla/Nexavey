#include "nex_swapchain.hpp"

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace nex {

    NexSwapChain::NexSwapChain(NexDevice& deviceRef, VkExtent2D extent) : m_device{deviceRef}, m_window_extent{extent} {
        init();
    }

    NexSwapChain::NexSwapChain(NexDevice& deviceRef, VkExtent2D extent, std::shared_ptr<NexSwapChain> oldSwapchain) : m_device{deviceRef}, m_window_extent{extent}, m_old_swap_chain(oldSwapchain) {
        init();

        m_old_swap_chain.reset();
    }

    void NexSwapChain::init() {
        createSwapChain();
        createImageViews();
        createRenderPass();
        createColorResources();
        createDepthResources();
        createFramebuffers();
        createSyncObjects();
    }

    NexSwapChain::~NexSwapChain() {
        for (auto image_view : m_swap_chain_image_views) {
            vkDestroyImageView(m_device.device(), image_view, nullptr);
        }
        m_swap_chain_image_views.clear();

        if (m_swap_chain != nullptr) {
            vkDestroySwapchainKHR(m_device.device(), m_swap_chain, nullptr);
            m_swap_chain = nullptr;
        }

        for (int i = 0; i < m_depth_images.size(); i++) {
            vkDestroyImageView(m_device.device(), m_depth_image_views[i], nullptr);
            vkDestroyImage(m_device.device(), m_depth_images[i], nullptr);
            vkFreeMemory(m_device.device(), m_depth_image_memorys[i], nullptr);
        }

        for (int i = 0; i < m_color_images.size(); i++) {
            vkDestroyImageView(m_device.device(), m_color_image_views[i], nullptr);
            vkDestroyImage(m_device.device(), m_color_images[i], nullptr);
            vkFreeMemory(m_device.device(), m_color_image_memorys[i], nullptr);
        }

        for (auto framebuffer : m_swap_chain_framebuffers) {
            vkDestroyFramebuffer(m_device.device(), framebuffer, nullptr);
        }

        vkDestroyRenderPass(m_device.device(), m_render_pass, nullptr);

        // cleanup synchronization objects
        for (size_t i = 0; i < max_frames_in_flight; i++) {
            vkDestroySemaphore(m_device.device(), m_render_finished_semaphores[i], nullptr);
            vkDestroySemaphore(m_device.device(), m_image_available_semaphores[i], nullptr);
            vkDestroyFence(m_device.device(), m_in_flight_fences[i], nullptr);
        }
    }

    VkResult NexSwapChain::acquireNextImage(uint32_t* imageIndex) {
        vkWaitForFences(m_device.device(), 1, &m_in_flight_fences[m_current_frame], VK_TRUE, std::numeric_limits<uint64_t>::max());

        VkResult result = vkAcquireNextImageKHR(m_device.device(), m_swap_chain, std::numeric_limits<uint64_t>::max(),
                                                m_image_available_semaphores[m_current_frame],  // must be a not signaled semaphore
                                                VK_NULL_HANDLE, imageIndex);

        return result;
    }

    VkResult NexSwapChain::submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex) {
        if (m_images_in_flight[*imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(m_device.device(), 1, &m_images_in_flight[*imageIndex], VK_TRUE, UINT64_MAX);
        }
        m_images_in_flight[*imageIndex] = m_in_flight_fences[m_current_frame];

        VkSubmitInfo submit_info = {};
        submit_info.sType        = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore          wait_semaphores[] = {m_image_available_semaphores[m_current_frame]};
        VkPipelineStageFlags wait_stages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submit_info.waitSemaphoreCount         = 1;
        submit_info.pWaitSemaphores            = wait_semaphores;
        submit_info.pWaitDstStageMask          = wait_stages;

        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers    = buffers;

        VkSemaphore signal_semaphores[]  = {m_render_finished_semaphores[m_current_frame]};
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores    = signal_semaphores;

        vkResetFences(m_device.device(), 1, &m_in_flight_fences[m_current_frame]);
        if (vkQueueSubmit(m_device.graphicsQueue(), 1, &submit_info, m_in_flight_fences[m_current_frame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR present_info = {};
        present_info.sType            = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores    = signal_semaphores;

        VkSwapchainKHR swap_chains[] = {m_swap_chain};
        present_info.swapchainCount  = 1;
        present_info.pSwapchains     = swap_chains;

        present_info.pImageIndices = imageIndex;

        auto result = vkQueuePresentKHR(m_device.presentQueue(), &present_info);

        m_current_frame = (m_current_frame + 1) % max_frames_in_flight;

        return result;
    }

    void NexSwapChain::createSwapChain() {
        SwapChainSupportDetails swap_chain_support = m_device.getSwapChainSupport();

        VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat(swap_chain_support.m_formats);
        VkPresentModeKHR   present_mode   = chooseSwapPresentMode(swap_chain_support.m_present_modes);
        VkExtent2D         extent         = chooseSwapExtent(swap_chain_support.m_capabilities);

        uint32_t image_count = swap_chain_support.m_capabilities.minImageCount + 1;
        if (swap_chain_support.m_capabilities.maxImageCount > 0 && image_count > swap_chain_support.m_capabilities.maxImageCount) {
            image_count = swap_chain_support.m_capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR create_info = {};
        create_info.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface                  = m_device.surface();

        create_info.minImageCount    = image_count;
        create_info.imageFormat      = surface_format.format;
        create_info.imageColorSpace  = surface_format.colorSpace;
        create_info.imageExtent      = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices                = m_device.findPhysicalQueueFamilies();
        uint32_t           queue_family_indices[] = {indices.m_graphics_family, indices.m_present_family};

        if (indices.m_graphics_family != indices.m_present_family) {
            create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices   = queue_family_indices;
        } else {
            create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0;        // Optional
            create_info.pQueueFamilyIndices   = nullptr;  // Optional
        }

        create_info.preTransform   = swap_chain_support.m_capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        create_info.presentMode = present_mode;
        create_info.clipped     = VK_TRUE;

        create_info.oldSwapchain = m_old_swap_chain ? m_old_swap_chain->m_swap_chain : VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_device.device(), &create_info, nullptr, &m_swap_chain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        // we only specified a minimum number of images in the swap chain, so the implementation is
        // allowed to create a swap chain with more. That's why we'll first query the final number of
        // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
        // retrieve the handles.
        vkGetSwapchainImagesKHR(m_device.device(), m_swap_chain, &image_count, nullptr);
        m_swap_chain_images.resize(image_count);
        vkGetSwapchainImagesKHR(m_device.device(), m_swap_chain, &image_count, m_swap_chain_images.data());

        m_swap_chain_image_format = surface_format.format;
        m_swap_chain_extent       = extent;
    }

    void NexSwapChain::createImageViews() {
        m_swap_chain_image_views.resize(m_swap_chain_images.size());
        for (size_t i = 0; i < m_swap_chain_images.size(); i++) {
            VkImageViewCreateInfo view_info{};
            view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            view_info.image                           = m_swap_chain_images[i];
            view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            view_info.format                          = m_swap_chain_image_format;
            view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            view_info.subresourceRange.baseMipLevel   = 0;
            view_info.subresourceRange.levelCount     = 1;
            view_info.subresourceRange.baseArrayLayer = 0;
            view_info.subresourceRange.layerCount     = 1;

            if (vkCreateImageView(m_device.device(), &view_info, nullptr, &m_swap_chain_image_views[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }

    void NexSwapChain::createRenderPass() {
        VkAttachmentDescription color_attachment = {};
        color_attachment.format                  = getSwapChainImageFormat();
        color_attachment.samples                 = m_device.getMaxUsableSamples();
        color_attachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout             = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depth_attachment{};
        depth_attachment.format         = findDepthFormat();
        depth_attachment.samples        = m_device.getMaxUsableSamples();
        depth_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription color_attachment_resolve = {};
        color_attachment_resolve.format                  = getSwapChainImageFormat();
        color_attachment_resolve.samples                 = VK_SAMPLE_COUNT_1_BIT;
        color_attachment_resolve.loadOp                  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment_resolve.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_resolve.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment_resolve.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment_resolve.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment_resolve.finalLayout             = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref = {};
        color_attachment_ref.attachment            = 0;
        color_attachment_ref.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depth_attachment_ref{};
        depth_attachment_ref.attachment = 1;
        depth_attachment_ref.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference color_attachment_resolve_ref = {};
        color_attachment_resolve_ref.attachment            = 2;
        color_attachment_resolve_ref.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass    = {};
        subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount    = 1;
        subpass.pColorAttachments       = &color_attachment_ref;
        subpass.pDepthStencilAttachment = &depth_attachment_ref;
        subpass.pResolveAttachments     = &color_attachment_resolve_ref;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
        dependency.srcAccessMask       = 0;
        dependency.srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstSubpass          = 0;
        dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 3> attachments      = {color_attachment, depth_attachment, color_attachment_resolve};
        VkRenderPassCreateInfo                 render_pass_info = {};
        render_pass_info.sType                                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount                        = static_cast<uint32_t>(attachments.size());
        render_pass_info.pAttachments                           = attachments.data();
        render_pass_info.subpassCount                           = 1;
        render_pass_info.pSubpasses                             = &subpass;
        render_pass_info.dependencyCount                        = 1;
        render_pass_info.pDependencies                          = &dependency;

        if (vkCreateRenderPass(m_device.device(), &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void NexSwapChain::createFramebuffers() {
        m_swap_chain_framebuffers.resize(imageCount());
        for (size_t i = 0; i < imageCount(); i++) {
            std::array<VkImageView, 3> attachments = {m_color_image_views[i], m_depth_image_views[i], m_swap_chain_image_views[i]};

            VkExtent2D              swap_chain_extent = getSwapChainExtent();
            VkFramebufferCreateInfo framebuffer_info  = {};
            framebuffer_info.sType                    = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass               = m_render_pass;
            framebuffer_info.attachmentCount          = static_cast<uint32_t>(attachments.size());
            framebuffer_info.pAttachments             = attachments.data();
            framebuffer_info.width                    = swap_chain_extent.width;
            framebuffer_info.height                   = swap_chain_extent.height;
            framebuffer_info.layers                   = 1;

            if (vkCreateFramebuffer(m_device.device(), &framebuffer_info, nullptr, &m_swap_chain_framebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void NexSwapChain::createColorResources() {
        VkFormat   color_format      = getSwapChainImageFormat();
        VkExtent2D swap_chain_extent = getSwapChainExtent();

        m_color_images.resize(imageCount());
        m_color_image_memorys.resize(imageCount());
        m_color_image_views.resize(imageCount());

        for (int i = 0; i < m_color_images.size(); i++) {
            VkImageCreateInfo image_info{};
            image_info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_info.imageType     = VK_IMAGE_TYPE_2D;
            image_info.extent.width  = swap_chain_extent.width;
            image_info.extent.height = swap_chain_extent.height;
            image_info.extent.depth  = 1;
            image_info.mipLevels     = 1;
            image_info.arrayLayers   = 1;
            image_info.format        = color_format;
            image_info.tiling        = VK_IMAGE_TILING_OPTIMAL;
            image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            image_info.usage         = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            image_info.samples       = m_device.getMaxUsableSamples();
            image_info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
            image_info.flags         = 0;

            m_device.createImageWithInfo(image_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_color_images[i], m_color_image_memorys[i]);

            VkImageViewCreateInfo view_info{};
            view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            view_info.image                           = m_color_images[i];
            view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            view_info.format                          = color_format;
            view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            view_info.subresourceRange.baseMipLevel   = 0;
            view_info.subresourceRange.levelCount     = 1;
            view_info.subresourceRange.baseArrayLayer = 0;
            view_info.subresourceRange.layerCount     = 1;

            if (vkCreateImageView(m_device.device(), &view_info, nullptr, &m_color_image_views[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }

    void NexSwapChain::createDepthResources() {
        VkFormat depth_format        = findDepthFormat();
        m_swap_chain_depth_format    = depth_format;
        VkExtent2D swap_chain_extent = getSwapChainExtent();

        m_depth_images.resize(imageCount());
        m_depth_image_memorys.resize(imageCount());
        m_depth_image_views.resize(imageCount());

        for (int i = 0; i < m_depth_images.size(); i++) {
            VkImageCreateInfo image_info{};
            image_info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_info.imageType     = VK_IMAGE_TYPE_2D;
            image_info.extent.width  = swap_chain_extent.width;
            image_info.extent.height = swap_chain_extent.height;
            image_info.extent.depth  = 1;
            image_info.mipLevels     = 1;
            image_info.arrayLayers   = 1;
            image_info.format        = depth_format;
            image_info.tiling        = VK_IMAGE_TILING_OPTIMAL;
            image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            image_info.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            image_info.samples       = m_device.getMaxUsableSamples();
            image_info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
            image_info.flags         = 0;

            m_device.createImageWithInfo(image_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depth_images[i], m_depth_image_memorys[i]);

            VkImageViewCreateInfo view_info{};
            view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            view_info.image                           = m_depth_images[i];
            view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            view_info.format                          = depth_format;
            view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
            view_info.subresourceRange.baseMipLevel   = 0;
            view_info.subresourceRange.levelCount     = 1;
            view_info.subresourceRange.baseArrayLayer = 0;
            view_info.subresourceRange.layerCount     = 1;

            if (vkCreateImageView(m_device.device(), &view_info, nullptr, &m_depth_image_views[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }

    void NexSwapChain::createSyncObjects() {
        m_image_available_semaphores.resize(max_frames_in_flight);
        m_render_finished_semaphores.resize(max_frames_in_flight);
        m_in_flight_fences.resize(max_frames_in_flight);
        m_images_in_flight.resize(imageCount(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphore_info = {};
        semaphore_info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info = {};
        fence_info.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags             = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < max_frames_in_flight; i++) {
            if (vkCreateSemaphore(m_device.device(), &semaphore_info, nullptr, &m_image_available_semaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_device.device(), &semaphore_info, nullptr, &m_render_finished_semaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_device.device(), &fence_info, nullptr, &m_in_flight_fences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    VkSurfaceFormatKHR NexSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& available_format : availableFormats) {
            if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return available_format;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR NexSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& available_present_mode : availablePresentModes) {
            if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                std::cout << "Present mode: Mailbox" << std::endl;
                return available_present_mode;
            }
        }

        // for (const auto &availablePresentMode : availablePresentModes) {
        //   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
        //     std::cout << "Present mode: Immediate" << std::endl;
        //     return availablePresentMode;
        //   }
        // }

        std::cout << "Present mode: V-Sync" << std::endl;
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D NexSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actual_extent = m_window_extent;
            actual_extent.width      = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
            actual_extent.height     = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual_extent.height));

            return actual_extent;
        }
    }

    VkFormat NexSwapChain::findDepthFormat() {
        return m_device.findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

}  // namespace nex
