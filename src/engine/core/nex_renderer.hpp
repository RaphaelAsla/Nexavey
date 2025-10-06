#pragma once

#include <cassert>
#include <memory>

#include "nex_device.hpp"
#include "nex_swapchain.hpp"
#include "nex_window.hpp"

namespace nex {
    class NexRenderer {
      public:
        NexRenderer(NexWindow& window, NexDevice& device);
        ~NexRenderer();

        NexRenderer(const NexRenderer&)            = delete;
        NexRenderer& operator=(const NexRenderer&) = delete;

        VkRenderPass getSwapChainRenderPass() const {
            return m_swap_chain->getRenderPass();
        }

        float getAspectRatio() const {
            return m_swap_chain->extentAspectRatio();
        }

        bool isFrameInProgress() const {
            return m_is_frame_started;
        }

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(m_is_frame_started && "Cannot get command buffer when frame is not in progress");
            return m_command_buffers[m_current_frame_index];
        }

        int getFrameIndex() const {
            assert(m_is_frame_started && "Cannot get frame index when frame not in progress");
            return m_current_frame_index;
        }

        VkCommandBuffer beginFrame();
        void            endFrame();

        void beginSwapChainRenderPass(VkCommandBuffer command_buffer);
        void endSwapChainRenderPass(VkCommandBuffer command_buffer);

      private:
        void recreateSwapChain();
        void createCommandBuffers();
        void freeCommandBuffers();

        uint32_t m_current_image_index = 0;
        int      m_current_frame_index = 0;
        bool     m_is_frame_started    = false;

        NexWindow&                    m_window;
        NexDevice&                    m_device;
        std::vector<VkCommandBuffer>  m_command_buffers;
        std::unique_ptr<NexSwapChain> m_swap_chain;
    };
}  // namespace nex
