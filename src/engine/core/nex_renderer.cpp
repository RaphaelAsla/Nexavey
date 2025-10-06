#include "nex_renderer.hpp"

#include <array>

namespace nex {
    NexRenderer::NexRenderer(NexWindow& window, NexDevice& device) : m_window(window), m_device(device) {
        recreateSwapChain();
        createCommandBuffers();
    }

    NexRenderer::~NexRenderer() {
        freeCommandBuffers();
    }

    VkCommandBuffer NexRenderer::beginFrame() {
        assert(!m_is_frame_started && "Frame already in progress");

        auto result = m_swap_chain->acquireNextImage(&m_current_image_index);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.wasWindowResized()) {
            m_window.resetWindowResizeFlag();
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to acquire swap chain image!");
        }

        m_is_frame_started = true;

        auto command_buffer = getCurrentCommandBuffer();

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        return command_buffer;
    }

    void NexRenderer::endFrame() {
        assert(m_is_frame_started && "Cannot end frame when frame is not in progress");

        auto command_buffer = getCurrentCommandBuffer();

        if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer!");
        }

        auto result = m_swap_chain->submitCommandBuffers(&command_buffer, &m_current_image_index);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.wasWindowResized()) {
            m_window.resetWindowResizeFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit command buffer!");
        }

        m_is_frame_started    = false;
        m_current_frame_index = (m_current_frame_index + 1) % NexSwapChain::max_frames_in_flight;
    }

    void NexRenderer::beginSwapChainRenderPass(VkCommandBuffer command_buffer) {
        assert(m_is_frame_started && "Cannot begin render pass when frame is not in progress");
        assert(command_buffer == getCurrentCommandBuffer() && "Command buffer must be the current command buffer");

        VkRenderPassBeginInfo render_pass_info = {};
        render_pass_info.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass            = m_swap_chain->getRenderPass();
        render_pass_info.framebuffer           = m_swap_chain->getFrameBuffer(m_current_image_index);

        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = m_swap_chain->getSwapChainExtent();

        std::array<VkClearValue, 2> clear_values = {};
        clear_values[0].color                    = {{0.01f, 0.01f, 0.01f, 1.0f}};
        clear_values[1].depthStencil             = {1.0f, 0};
        render_pass_info.clearValueCount         = static_cast<uint32_t>(clear_values.size());
        render_pass_info.pClearValues            = clear_values.data();

        vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport = {};
        viewport.x          = 0.0f;
        viewport.y          = 0.0f;
        viewport.width      = static_cast<float>(m_swap_chain->getSwapChainExtent().width);
        viewport.height     = static_cast<float>(m_swap_chain->getSwapChainExtent().height);
        viewport.minDepth   = 0.0f;
        viewport.maxDepth   = 1.0f;
        VkRect2D scissor    = {{0, 0}, m_swap_chain->getSwapChainExtent()};

        vkCmdSetViewport(command_buffer, 0, 1, &viewport);
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);
    }
    void NexRenderer::endSwapChainRenderPass(VkCommandBuffer command_buffer) {
        assert(m_is_frame_started && "Cannot end render pass when frame is not in progress");
        assert(command_buffer == getCurrentCommandBuffer() && "Command buffer must be the current command buffer");

        vkCmdEndRenderPass(command_buffer);
    }

    void NexRenderer::recreateSwapChain() {
        auto extent = m_window.getExtent();

        while (extent.width == 0 || extent.height == 0) {
            extent = m_window.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_device.device());

        if (m_swap_chain == nullptr) {
            m_swap_chain = std::make_unique<NexSwapChain>(m_device, extent);
        } else {
            std::shared_ptr<NexSwapChain> old_swap_chain = std::move(m_swap_chain);
            m_swap_chain                                 = std::make_unique<NexSwapChain>(m_device, extent, old_swap_chain);

            if (!old_swap_chain->compareSwapFormats(*m_swap_chain.get())) {
                throw std::runtime_error("Swap chain image(or depth) format has changed!");
            }
        }
    }

    void NexRenderer::createCommandBuffers() {
        m_command_buffers.resize(NexSwapChain::max_frames_in_flight);

        VkCommandBufferAllocateInfo alloc_info = {};
        alloc_info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool                 = m_device.getCommandPool();
        alloc_info.commandBufferCount          = static_cast<uint32_t>(m_command_buffers.size());

        if (vkAllocateCommandBuffers(m_device.device(), &alloc_info, m_command_buffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }

    void NexRenderer::freeCommandBuffers() {
        vkFreeCommandBuffers(m_device.device(), m_device.getCommandPool(), static_cast<uint32_t>(m_command_buffers.size()), m_command_buffers.data());
        m_command_buffers.clear();
    }

}  // namespace nex
