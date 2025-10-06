#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "../core/nex_device.hpp"

namespace nex {
    class NexDescriptorSetLayout {
      public:
        class Builder {
          public:
            Builder(NexDevice& device) : m_device{device} {}

            Builder&                                addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);
            std::unique_ptr<NexDescriptorSetLayout> build() const;

          private:
            NexDevice&                                                 m_device;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings = {};
        };

        NexDescriptorSetLayout(NexDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~NexDescriptorSetLayout();
        NexDescriptorSetLayout(const NexDescriptorSetLayout&)            = delete;
        NexDescriptorSetLayout& operator=(const NexDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const {
            return m_descriptor_set_layout;
        }

      private:
        NexDevice&                                                 m_device;
        VkDescriptorSetLayout                                      m_descriptor_set_layout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;

        friend class NexDescriptorWriter;
    };

    class NexDescriptorPool {
      public:
        class Builder {
          public:
            Builder(NexDevice& device) : m_device{device} {}

            Builder&                           addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder&                           setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder&                           setMaxSets(uint32_t count);
            std::unique_ptr<NexDescriptorPool> build() const;

          private:
            NexDevice&                        m_device;
            std::vector<VkDescriptorPoolSize> m_pool_sizes{};
            uint32_t                          m_max_sets   = 1000;
            VkDescriptorPoolCreateFlags       m_pool_flags = 0;
        };

        NexDescriptorPool(NexDevice& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~NexDescriptorPool();
        NexDescriptorPool(const NexDescriptorPool&)            = delete;
        NexDescriptorPool& operator=(const NexDescriptorPool&) = delete;

        bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

      private:
        NexDevice&       m_device;
        VkDescriptorPool m_descriptor_pool;

        friend class NexDescriptorWriter;
    };

    class NexDescriptorWriter {
      public:
        NexDescriptorWriter(NexDescriptorSetLayout& setLayout, NexDescriptorPool& pool);

        NexDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        NexDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

      private:
        NexDescriptorSetLayout&           m_set_layout;
        NexDescriptorPool&                m_pool;
        std::vector<VkWriteDescriptorSet> m_writes;
    };
}  // namespace nex
