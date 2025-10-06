#include "shadowmap_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace nex {
    ShadowSystem::ShadowSystem(NexDevice& device) : m_device(device) {
        m_shadow_map = std::make_unique<NexShadowMap>(device, 4096, 4096);
        createPipelineLayout();
        createPipeline();
    }

    ShadowSystem::~ShadowSystem() {
        vkDestroyPipelineLayout(m_device.device(), m_pipeline_layout, nullptr);
    }

    void ShadowSystem::renderShadowMap(NexFrameInfo& frame_info) {
        VkClearValue clear_value = {};
        clear_value.depthStencil = {1.0f, 0};

        VkRenderPassBeginInfo render_pass_info = {};
        render_pass_info.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass            = m_shadow_map->getRenderPass();
        render_pass_info.framebuffer           = m_shadow_map->getFrameBuffer();
        render_pass_info.renderArea.offset     = {0, 0};
        render_pass_info.renderArea.extent     = {m_shadow_map->getWidth(), m_shadow_map->getHeight()};
        render_pass_info.clearValueCount       = 1;
        render_pass_info.pClearValues          = &clear_value;

        vkCmdBeginRenderPass(frame_info.m_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport = {};
        viewport.x          = 0.0f;
        viewport.y          = 0.0f;
        viewport.width      = static_cast<float>(m_shadow_map->getWidth());
        viewport.height     = static_cast<float>(m_shadow_map->getHeight());
        viewport.minDepth   = 0.0f;
        viewport.maxDepth   = 1.0f;
        VkRect2D scissor    = {{0, 0}, {m_shadow_map->getWidth(), m_shadow_map->getHeight()}};

        vkCmdSetViewport(frame_info.m_command_buffer, 0, 1, &viewport);
        vkCmdSetScissor(frame_info.m_command_buffer, 0, 1, &scissor);

        vkCmdSetDepthBias(frame_info.m_command_buffer, 1.25f, 0.0f, 1.75f);

        m_shadow_pipeline->bind(frame_info.m_command_buffer);

        glm::vec3 light_pos        = {2.0f, -2.0f, -2.0f};
        glm::vec3 scene_center     = {0.0f, 0.0f, 0.0f};
        glm::mat4 light_view       = glm::lookAt(light_pos, scene_center, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 7.5f);
        m_light_space_matrix       = light_projection * light_view;

        for (auto& kv : frame_info.m_entities) {
            auto& entity = kv.second;

            if (entity.m_model == nullptr) {
                continue;
            }

            ShadowPushConstantsData push{};
            push.m_light_space_model_matrix = m_light_space_matrix * entity.m_transform.mat4();

            vkCmdPushConstants(frame_info.m_command_buffer, m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ShadowPushConstantsData), &push);

            entity.m_model->bind(frame_info.m_command_buffer, 0);
            entity.m_model->draw(frame_info.m_command_buffer, 0);
        }

        vkCmdEndRenderPass(frame_info.m_command_buffer);
    }

    VkDescriptorImageInfo ShadowSystem::getShadowMapDescriptor() {
        return m_shadow_map->getDescriptorInfo();
    }

    glm::mat4 ShadowSystem::getLightSpaceMatrix() {
        return m_light_space_matrix;
    }

    void ShadowSystem::createPipelineLayout() {
        VkPushConstantRange push_constant_range = {};
        push_constant_range.stageFlags          = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_range.offset              = 0;
        push_constant_range.size                = sizeof(ShadowPushConstantsData);

        VkPipelineLayoutCreateInfo pipeline_layout_info = {};
        pipeline_layout_info.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount             = 0;
        pipeline_layout_info.pSetLayouts                = nullptr;
        pipeline_layout_info.pushConstantRangeCount     = 1;
        pipeline_layout_info.pPushConstantRanges        = &push_constant_range;

        if (vkCreatePipelineLayout(m_device.device(), &pipeline_layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void ShadowSystem::createPipeline() {
        PipelineConfigInfo pipeline_config = {};
        NexPipeline::defaultPipelineConfigInfo(pipeline_config);

        pipeline_config.m_depth_stencil_info.depthTestEnable       = VK_TRUE;
        pipeline_config.m_depth_stencil_info.depthWriteEnable      = VK_TRUE;
        pipeline_config.m_depth_stencil_info.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
        pipeline_config.m_depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
        pipeline_config.m_depth_stencil_info.minDepthBounds        = 0.0f;  // Optional
        pipeline_config.m_depth_stencil_info.maxDepthBounds        = 1.0f;  // Optional
        pipeline_config.m_depth_stencil_info.stencilTestEnable     = VK_FALSE;
        pipeline_config.m_multisample_info.rasterizationSamples    = VK_SAMPLE_COUNT_1_BIT;
        pipeline_config.m_render_pass                              = m_shadow_map->getRenderPass();
        pipeline_config.m_pipeline_layout                          = m_pipeline_layout;
        m_shadow_pipeline = std::make_unique<NexPipeline>(m_device, "./shaders_compiled/shadowmap_shader.vert.spv", "./shaders_compiled/shadowmap_shader.frag.spv", pipeline_config);
    }

}  // namespace nex
