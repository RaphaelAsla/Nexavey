
#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace nex {
    struct SimplePushConstantsData {
        glm::mat4 m_model_matrix       = {1.0f};
        glm::mat4 m_normal_matrix      = {1.0f};
        glm::mat4 m_light_space_matrix = {1.0f};
        int       m_material_index     = 0;
    };

    SimpleRenderSystem::SimpleRenderSystem(NexDevice& device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout) : m_device(device) {
        m_default_texture = NexTexture::createTextureFromFile(m_device, "../textures/missing.png");
        m_default_texture->updateDescriptor();

        createTextureDescriptorLayout();
        createShadowDescriptorLayout();

        createPipelineLayout(global_set_layout);
        createPipeline(render_pass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(m_device.device(), m_pipeline_layout, nullptr);
    }

    void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout global_set_layout) {
        VkPushConstantRange push_constant_range = {};
        push_constant_range.stageFlags          = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_range.offset              = 0;
        push_constant_range.size                = sizeof(SimplePushConstantsData);

        std::vector<VkDescriptorSetLayout> descriptor_set_layouts = {global_set_layout, m_texture_set_layout->getDescriptorSetLayout(), m_shadow_set_layout->getDescriptorSetLayout()};

        VkPipelineLayoutCreateInfo pipeline_layout_info = {};
        pipeline_layout_info.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount             = static_cast<uint32_t>(descriptor_set_layouts.size());
        pipeline_layout_info.pSetLayouts                = descriptor_set_layouts.data();
        pipeline_layout_info.pushConstantRangeCount     = 1;
        pipeline_layout_info.pPushConstantRanges        = &push_constant_range;

        if (vkCreatePipelineLayout(m_device.device(), &pipeline_layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::createPipeline(VkRenderPass render_pass) {
        PipelineConfigInfo pipeline_config = {};
        NexPipeline::defaultPipelineConfigInfo(pipeline_config);

        pipeline_config.m_multisample_info.rasterizationSamples = m_device.getMaxUsableSamples();
        pipeline_config.m_render_pass                           = render_pass;
        pipeline_config.m_pipeline_layout                       = m_pipeline_layout;
        m_pipeline = std::make_unique<NexPipeline>(m_device, "./shaders_compiled/simple_shader.vert.spv", "./shaders_compiled/simple_shader.frag.spv", pipeline_config);
    }

    void SimpleRenderSystem::createTextureDescriptorLayout() {
        m_texture_set_layout = NexDescriptorSetLayout::Builder(m_device).addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).build();
    }

    void SimpleRenderSystem::createShadowDescriptorLayout() {
        m_shadow_set_layout = NexDescriptorSetLayout::Builder(m_device).addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).build();
    }

    void SimpleRenderSystem::renderEntities(NexFrameInfo& frame_info, VkDescriptorImageInfo shadow_map_descriptor, glm::mat4 light_space_matrix) {
        m_pipeline->bind(frame_info.m_command_buffer);

        vkCmdBindDescriptorSets(frame_info.m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 0, 1, &frame_info.m_global_descriptor_set, 0, nullptr);

        VkDescriptorSet shadow_descriptor_set;
        NexDescriptorWriter(*m_shadow_set_layout, frame_info.m_frame_descriptor_pool).writeImage(0, &shadow_map_descriptor).build(shadow_descriptor_set);
        vkCmdBindDescriptorSets(frame_info.m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 2, 1, &shadow_descriptor_set, 0, nullptr);

        for (auto& [id, entity] : frame_info.m_entities) {
            if (!entity.m_model) {
                continue;
            }

            VkDescriptorSet texture_descriptor_set;
            auto            texture_info = entity.m_texture == nullptr ? m_default_texture->getDescriptorInfo() : entity.m_texture->getDescriptorInfo();
            NexDescriptorWriter(*m_texture_set_layout, frame_info.m_frame_descriptor_pool).writeImage(0, &texture_info).build(texture_descriptor_set);
            vkCmdBindDescriptorSets(frame_info.m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 1, 1, &texture_descriptor_set, 0, nullptr);

            SimplePushConstantsData push = {};

            push.m_model_matrix       = entity.m_transform.mat4();
            push.m_normal_matrix      = entity.m_transform.normalMatrix();
            push.m_light_space_matrix = light_space_matrix;
            push.m_material_index     = entity.m_material_index;

            vkCmdPushConstants(frame_info.m_command_buffer, m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantsData), &push);
            entity.m_model->bind(frame_info.m_command_buffer, 0);
            entity.m_model->draw(frame_info.m_command_buffer, 0);
        }
    }

}  // namespace nex
