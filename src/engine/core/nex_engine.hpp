#pragma once

#include <memory>

#include "../graphics/nex_descriptors.hpp"
#include "nex_device.hpp"
#include "../scene/nex_entity.hpp"
#include "nex_renderer.hpp"
#include "nex_window.hpp"

namespace nex {
    class NexEngine {
      public:
        static constexpr int width  = 1500;
        static constexpr int height = 1000;

        NexEngine();
        ~NexEngine();

        NexEngine(const NexEngine&)            = delete;
        NexEngine& operator=(const NexEngine&) = delete;

        void run();

      private:
        void loadEntities();

        NexWindow   m_window   = {"First app", width, height};
        NexDevice   m_device   = m_window;
        NexRenderer m_renderer = {m_window, m_device};

        // note: order of declaration matters
        std::unique_ptr<NexDescriptorPool>              m_descriptor_pool = {};
        std::vector<std::unique_ptr<NexDescriptorPool>> m_frame_descriptor_pools;
        NexEntity::Map                                  m_entities = {};
    };
}  // namespace nex
