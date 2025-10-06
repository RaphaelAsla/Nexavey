#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "nex_device.hpp"

namespace nex {

    class NexSwapChain {
      public:
        static constexpr int max_frames_in_flight = 2;

        NexSwapChain(NexDevice& deviceRef, VkExtent2D windowExtent);
        NexSwapChain(NexDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<NexSwapChain> oldSwapChain);
        ~NexSwapChain();

        NexSwapChain(const NexSwapChain&)            = delete;
        NexSwapChain& operator=(const NexSwapChain&) = delete;

        VkFramebuffer getFrameBuffer(int index) {
            return m_swap_chain_framebuffers[index];
        }
        VkRenderPass getRenderPass() {
            return m_render_pass;
        }
        VkImageView getImageView(int index) {
            return m_swap_chain_image_views[index];
        }
        size_t imageCount() {
            return m_swap_chain_images.size();
        }
        VkFormat getSwapChainImageFormat() {
            return m_swap_chain_image_format;
        }
        VkExtent2D getSwapChainExtent() {
            return m_swap_chain_extent;
        }
        uint32_t width() {
            return m_swap_chain_extent.width;
        }
        uint32_t height() {
            return m_swap_chain_extent.height;
        }

        float extentAspectRatio() {
            return static_cast<float>(m_swap_chain_extent.width) / static_cast<float>(m_swap_chain_extent.height);
        }
        VkFormat findDepthFormat();

        VkResult acquireNextImage(uint32_t* imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

        bool compareSwapFormats(const NexSwapChain& swapChain) const {
            return swapChain.m_swap_chain_image_format == m_swap_chain_image_format && swapChain.m_swap_chain_depth_format == m_swap_chain_depth_format;
        }

      private:
        void init();
        void createSwapChain();
        void createImageViews();
        void createColorResources();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR   chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D         chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        VkFormat   m_swap_chain_image_format;
        VkFormat   m_swap_chain_depth_format;
        VkExtent2D m_swap_chain_extent;

        std::vector<VkFramebuffer> m_swap_chain_framebuffers;
        VkRenderPass               m_render_pass;

        std::vector<VkImage>        m_depth_images;
        std::vector<VkDeviceMemory> m_depth_image_memorys;
        std::vector<VkImageView>    m_depth_image_views;

        std::vector<VkImage>        m_color_images;
        std::vector<VkDeviceMemory> m_color_image_memorys;
        std::vector<VkImageView>    m_color_image_views;

        std::vector<VkImage>     m_swap_chain_images;
        std::vector<VkImageView> m_swap_chain_image_views;

        NexDevice& m_device;
        VkExtent2D m_window_extent;

        VkSwapchainKHR                m_swap_chain;
        std::shared_ptr<NexSwapChain> m_old_swap_chain;

        std::vector<VkSemaphore> m_image_available_semaphores;
        std::vector<VkSemaphore> m_render_finished_semaphores;
        std::vector<VkFence>     m_in_flight_fences;
        std::vector<VkFence>     m_images_in_flight;
        size_t                   m_current_frame = 0;
    };

}  // namespace nex
