#pragma once

#include <memory>

#include "../graphics/nex_descriptors.hpp"
#include "../core/nex_device.hpp"
#include "../scene/nex_frame_info.hpp"
#include "../graphics/nex_pipeline.hpp"
#include "../graphics/nex_texture.hpp"

namespace nex {
    class SimpleRenderSystem {
      public:
        SimpleRenderSystem(NexDevice& device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&)            = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

        void renderEntities(NexFrameInfo& frame_info, VkDescriptorImageInfo shadow_map_descriptor, glm::mat4 light_space_matrix);

      private:
        void createPipelineLayout(VkDescriptorSetLayout global_set_layout);
        void createPipeline(VkRenderPass render_pass);
        void createTextureDescriptorLayout();
        void createShadowDescriptorLayout();

        NexDevice& m_device;

        VkPipelineLayout             m_pipeline_layout;
        std::unique_ptr<NexPipeline> m_pipeline;

        std::shared_ptr<NexTexture>             m_default_texture;
        std::unique_ptr<NexDescriptorSetLayout> m_texture_set_layout;
        std::unique_ptr<NexDescriptorSetLayout> m_shadow_set_layout;
    };
}  // namespace nex
