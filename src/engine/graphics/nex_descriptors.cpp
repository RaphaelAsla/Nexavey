
#include "nex_descriptors.hpp"

#include <cassert>
#include <stdexcept>

namespace nex {

    NexDescriptorSetLayout::Builder& NexDescriptorSetLayout::Builder::addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count) {
        assert(m_bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layout_binding{};
        layout_binding.binding         = binding;
        layout_binding.descriptorType  = descriptorType;
        layout_binding.descriptorCount = count;
        layout_binding.stageFlags      = stageFlags;
        m_bindings[binding]            = layout_binding;
        return *this;
    }

    std::unique_ptr<NexDescriptorSetLayout> NexDescriptorSetLayout::Builder::build() const {
        return std::make_unique<NexDescriptorSetLayout>(m_device, m_bindings);
    }

    NexDescriptorSetLayout::NexDescriptorSetLayout(NexDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings) : m_device{device}, m_bindings{bindings} {
        std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings{};
        for (auto kv : bindings) {
            set_layout_bindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
        descriptor_set_layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_set_layout_info.bindingCount = static_cast<uint32_t>(set_layout_bindings.size());
        descriptor_set_layout_info.pBindings    = set_layout_bindings.data();

        if (vkCreateDescriptorSetLayout(device.device(), &descriptor_set_layout_info, nullptr, &m_descriptor_set_layout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    NexDescriptorSetLayout::~NexDescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(m_device.device(), m_descriptor_set_layout, nullptr);
    }

    NexDescriptorPool::Builder& NexDescriptorPool::Builder::addPoolSize(VkDescriptorType descriptorType, uint32_t count) {
        m_pool_sizes.push_back({descriptorType, count});
        return *this;
    }

    NexDescriptorPool::Builder& NexDescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags) {
        m_pool_flags = flags;
        return *this;
    }
    NexDescriptorPool::Builder& NexDescriptorPool::Builder::setMaxSets(uint32_t count) {
        m_max_sets = count;
        return *this;
    }

    std::unique_ptr<NexDescriptorPool> NexDescriptorPool::Builder::build() const {
        return std::make_unique<NexDescriptorPool>(m_device, m_max_sets, m_pool_flags, m_pool_sizes);
    }

    NexDescriptorPool::NexDescriptorPool(NexDevice& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes) : m_device{device} {
        VkDescriptorPoolCreateInfo descriptor_pool_info{};
        descriptor_pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptor_pool_info.pPoolSizes    = poolSizes.data();
        descriptor_pool_info.maxSets       = maxSets;
        descriptor_pool_info.flags         = poolFlags;

        if (vkCreateDescriptorPool(device.device(), &descriptor_pool_info, nullptr, &m_descriptor_pool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    NexDescriptorPool::~NexDescriptorPool() {
        vkDestroyDescriptorPool(m_device.device(), m_descriptor_pool, nullptr);
    }

    bool NexDescriptorPool::allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {
        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool     = m_descriptor_pool;
        alloc_info.pSetLayouts        = &descriptorSetLayout;
        alloc_info.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds a new pool whenever an old pool fills up
        if (vkAllocateDescriptorSets(m_device.device(), &alloc_info, &descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    void NexDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
        vkFreeDescriptorSets(m_device.device(), m_descriptor_pool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
    }

    void NexDescriptorPool::resetPool() {
        vkResetDescriptorPool(m_device.device(), m_descriptor_pool, 0);
    }

    NexDescriptorWriter::NexDescriptorWriter(NexDescriptorSetLayout& setLayout, NexDescriptorPool& pool) : m_set_layout{setLayout}, m_pool{pool} {}

    NexDescriptorWriter& NexDescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
        assert(m_set_layout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& binding_description = m_set_layout.m_bindings[binding];

        assert(binding_description.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType  = binding_description.descriptorType;
        write.dstBinding      = binding;
        write.pBufferInfo     = bufferInfo;
        write.descriptorCount = 1;

        m_writes.push_back(write);
        return *this;
    }

    NexDescriptorWriter& NexDescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo) {
        assert(m_set_layout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& binding_description = m_set_layout.m_bindings[binding];

        assert(binding_description.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType  = binding_description.descriptorType;
        write.dstBinding      = binding;
        write.pImageInfo      = imageInfo;
        write.descriptorCount = 1;

        m_writes.push_back(write);
        return *this;
    }

    bool NexDescriptorWriter::build(VkDescriptorSet& set) {
        bool success = m_pool.allocateDescriptor(m_set_layout.getDescriptorSetLayout(), set);
        if (!success) {
            return false;
        }
        overwrite(set);
        return true;
    }

    void NexDescriptorWriter::overwrite(VkDescriptorSet& set) {
        for (auto& write : m_writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(m_pool.m_device.device(), m_writes.size(), m_writes.data(), 0, nullptr);
    }

}  // namespace nex
