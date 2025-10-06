#pragma once

#include <string>
#include <vector>

#include "../core/nex_device.hpp"

namespace nex {

    struct PipelineConfigInfo {
        PipelineConfigInfo()                                     = default;
        PipelineConfigInfo(const PipelineConfigInfo&)            = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        std::vector<VkVertexInputBindingDescription>   m_binding_descriptions   = {};
        std::vector<VkVertexInputAttributeDescription> m_attribute_descriptions = {};
        VkPipelineViewportStateCreateInfo              m_viewport_info;
        VkPipelineInputAssemblyStateCreateInfo         m_input_assembly_info;
        VkPipelineRasterizationStateCreateInfo         m_rasterization_info;
        VkPipelineMultisampleStateCreateInfo           m_multisample_info;
        VkPipelineColorBlendAttachmentState            m_color_blend_attachment;
        VkPipelineColorBlendStateCreateInfo            m_color_blend_info;
        VkPipelineDepthStencilStateCreateInfo          m_depth_stencil_info;
        std::vector<VkDynamicState>                    m_dynamic_states;
        VkPipelineDynamicStateCreateInfo               m_dynamic_state_info;
        VkPipelineLayout                               m_pipeline_layout = nullptr;
        VkRenderPass                                   m_render_pass     = nullptr;
        uint32_t                                       m_subpass         = 0;
    };

    class NexPipeline {
      public:
        NexPipeline(NexDevice& device, const std::string& vert_shader_path, const std::string& frag_shader_path, const PipelineConfigInfo& config_info);
        ~NexPipeline();

        NexPipeline(const NexPipeline&)            = delete;
        NexPipeline& operator=(const NexPipeline&) = delete;

        void        bind(VkCommandBuffer command_buffer);
        static void defaultPipelineConfigInfo(PipelineConfigInfo& config_info);
        static void enableAlphaBlending(PipelineConfigInfo& config_info);

      private:
        static std::vector<char> readFile(const std::string& filepath);

        void createGraphicsPipeline(const std::string& vert_shader_path, const std::string& frag_shader_path, const PipelineConfigInfo& config_info);
        void createShaderModule(const std::vector<char>& code, VkShaderModule* shader_module);

        NexDevice&     m_device;
        VkPipeline     m_graphics_pipeline;
        VkShaderModule m_vert_shader_module;
        VkShaderModule m_frag_shader_module;
    };

}  // namespace nex
