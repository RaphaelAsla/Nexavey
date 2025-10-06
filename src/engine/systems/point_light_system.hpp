#pragma once

#include <memory>

#include "../core/nex_device.hpp"
#include "../scene/nex_frame_info.hpp"
#include "../graphics/nex_pipeline.hpp"

namespace nex {
    class PointLightSystem {
      public:
        PointLightSystem(NexDevice& device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem&)            = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        void update(NexFrameInfo& frame_info, GlobalUbo& ubo);
        void render(NexFrameInfo& frame_info);

      private:
        void createPipelineLayout(VkDescriptorSetLayout global_set_layout);
        void createPipeline(VkRenderPass render_pass);

        NexDevice& m_device;

        VkPipelineLayout             m_pipeline_layout;
        std::unique_ptr<NexPipeline> m_pipeline;
    };
}  // namespace nex
