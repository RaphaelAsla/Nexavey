#pragma once

#include "../core/nex_device.hpp"
#include "../scene/nex_frame_info.hpp"
#include "../graphics/nex_pipeline.hpp"
#include "../lighting/nex_shadowmap.hpp"

struct ShadowPushConstantsData {
    glm::mat4 m_light_space_model_matrix = {1.0f};
};

namespace nex {
    class ShadowSystem {
      public:
        ShadowSystem(NexDevice& device);
        ~ShadowSystem();

        void                  renderShadowMap(NexFrameInfo& frame_info);
        VkDescriptorImageInfo getShadowMapDescriptor();
        glm::mat4             getLightSpaceMatrix();

      private:
        void createPipelineLayout();
        void createPipeline();

        NexDevice& m_device;

        std::unique_ptr<NexShadowMap> m_shadow_map;
        std::unique_ptr<NexPipeline>  m_shadow_pipeline;
        VkPipelineLayout              m_pipeline_layout;
        glm::mat4                     m_light_space_matrix;
    };
};  // namespace nex
