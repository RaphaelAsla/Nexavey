
#include "point_light_system.hpp"

#include <cassert>
#include <map>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace nex {
    struct PointLightPushConstants {
        glm::vec4 m_poisition = {};
        glm::vec4 m_color     = {};
        float     m_radius;
    };

    PointLightSystem::PointLightSystem(NexDevice& device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout) : m_device(device) {
        createPipelineLayout(global_set_layout);
        createPipeline(render_pass);
    }

    PointLightSystem::~PointLightSystem() {
        vkDestroyPipelineLayout(m_device.device(), m_pipeline_layout, nullptr);
    }

    void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout global_set_layout) {
        VkPushConstantRange push_constant_range = {};
        push_constant_range.stageFlags          = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_range.offset              = 0;
        push_constant_range.size                = sizeof(PointLightPushConstants);

        std::vector<VkDescriptorSetLayout> descriptor_set_layouts = {global_set_layout};

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

    void PointLightSystem::createPipeline(VkRenderPass render_pass) {
        PipelineConfigInfo pipeline_config = {};
        NexPipeline::defaultPipelineConfigInfo(pipeline_config);
        NexPipeline::enableAlphaBlending(pipeline_config);

        pipeline_config.m_multisample_info.rasterizationSamples = m_device.getMaxUsableSamples();
        pipeline_config.m_attribute_descriptions.clear();
        pipeline_config.m_binding_descriptions.clear();
        pipeline_config.m_render_pass     = render_pass;
        pipeline_config.m_pipeline_layout = m_pipeline_layout;
        m_pipeline                        = std::make_unique<NexPipeline>(m_device, "./shaders_compiled/point_light.vert.spv", "./shaders_compiled/point_light.frag.spv", pipeline_config);
    }

    void PointLightSystem::update(NexFrameInfo& frame_info, GlobalUbo& ubo) {
        // auto rotation_angle = glm::rotate(glm::mat4{1.0f}, frame_info.m_frame_time, glm::vec3{0.0f, -1.0f, 0.0f});

        int light_index = 0;
        for (auto& [id, entity] : frame_info.m_entities) {
            if (!entity.m_point_light) {
                continue;
            }

            assert(light_index < MAX_LIGHTS && "Too many point lights!");

            // entity.m_transform.m_translation = glm::vec3{rotation_angle * glm::vec4{entity.m_transform.m_translation, 1.0f}};

            PointLight& light = ubo.m_point_lights[light_index];
            light.m_position  = glm::vec4(entity.m_transform.m_translation, 1.0f);
            light.m_color     = glm::vec4(entity.m_color, entity.m_point_light->m_intensity);
            light_index++;
        }
        ubo.m_light_count = light_index;
    }

    void PointLightSystem::render(NexFrameInfo& frame_info) {
        std::map<float, NexEntity::id_t> sorted_lights;

        for (auto& [id, entity] : frame_info.m_entities) {
            if (!entity.m_point_light) {
                continue;
            }

            auto  offset            = entity.m_transform.m_translation - frame_info.m_camera.getPosition();
            float distance          = glm::dot(offset, offset);
            sorted_lights[distance] = id;
        }

        m_pipeline->bind(frame_info.m_command_buffer);

        vkCmdBindDescriptorSets(frame_info.m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 0, 1, &frame_info.m_global_descriptor_set, 0, nullptr);

        for (auto it = sorted_lights.rbegin(); it != sorted_lights.rend(); ++it) {
            auto& entity = frame_info.m_entities.at(it->second);

            PointLightPushConstants push = {};
            push.m_poisition             = glm::vec4(entity.m_transform.m_translation, 1.0f);
            push.m_color                 = glm::vec4(entity.m_color, entity.m_point_light->m_intensity);
            push.m_radius                = entity.m_transform.m_scale.x;

            vkCmdPushConstants(frame_info.m_command_buffer, m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PointLightPushConstants), &push);
            vkCmdDraw(frame_info.m_command_buffer, 6, 1, 0, 0);
        }
    }

}  // namespace nex
