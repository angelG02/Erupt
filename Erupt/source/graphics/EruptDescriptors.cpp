#include "graphics/EruptDescriptors.h"

#include "core/Log.h"

// std
#include <cassert>
#include <stdexcept>

namespace Erupt
{
    // *************** Descriptor Set Layout Builder *********************

    EruptDescriptorSetLayout::Builder& EruptDescriptorSetLayout::Builder::AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count)
    {
        assert(m_Bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        m_Bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<EruptDescriptorSetLayout> EruptDescriptorSetLayout::Builder::Build() const
    {
        return std::make_unique<EruptDescriptorSetLayout>(m_Device, m_Bindings);
    }

    // *************** Descriptor Set Layout *********************

    EruptDescriptorSetLayout::EruptDescriptorSetLayout(EruptDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : m_Device{device}, m_Bindings{bindings}, m_DescriptorSetLayout{nullptr}
    {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (const auto& kv : m_Bindings)
        {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
            m_Device.Device(),
            &descriptorSetLayoutInfo,
            nullptr,
            &m_DescriptorSetLayout) != VK_SUCCESS)
        {
            ERUPT_CORE_ERROR("failed to create descriptor set layout!");
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }
    EruptDescriptorSetLayout::~EruptDescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(m_Device.Device(), m_DescriptorSetLayout, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    EruptDescriptorPool::Builder& EruptDescriptorPool::Builder::AddPoolSize(VkDescriptorType descriptorType, uint32_t count)
    {
        m_PoolSizes.push_back({ descriptorType, count });
        return *this;
    }

    EruptDescriptorPool::Builder& EruptDescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags flags)
    {
        m_PoolFlags = flags;
        return *this;
    }

    EruptDescriptorPool::Builder& EruptDescriptorPool::Builder::SetMaxSets(uint32_t count)
    {
        m_MaxSets = count;
        return *this;
    }

    std::unique_ptr<EruptDescriptorPool> EruptDescriptorPool::Builder::Build() const
    {
        return std::make_unique<EruptDescriptorPool>(m_Device, m_MaxSets, m_PoolFlags, m_PoolSizes);
    }

    // *************** Descriptor Pool *********************

    EruptDescriptorPool::EruptDescriptorPool(EruptDevice& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes)
        : m_Device{device}
    {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(m_Device.Device(), &descriptorPoolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
        {
            ERUPT_CORE_ERROR("failed to create descriptor pool!");
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    EruptDescriptorPool::~EruptDescriptorPool()
    {
        vkDestroyDescriptorPool(m_Device.Device(), m_DescriptorPool, nullptr);
    }

    bool EruptDescriptorPool::AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_DescriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(m_Device.Device(), &allocInfo, &descriptor) != VK_SUCCESS)
        {
            return false;
        }
        return true;
    }

    void EruptDescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
    {
        vkFreeDescriptorSets(
            m_Device.Device(),
            m_DescriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void EruptDescriptorPool::ResetPool()
    {
        vkResetDescriptorPool(m_Device.Device(), m_DescriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    EruptDescriptorWriter::EruptDescriptorWriter(EruptDescriptorSetLayout& setLayout, EruptDescriptorPool& pool)
        : m_SetLayout{setLayout}, m_Pool{pool}
    {
    }

    EruptDescriptorWriter& EruptDescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
    {
        assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = m_SetLayout.m_Bindings[binding];

        assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        m_Writes.push_back(write);
        return *this;
    }

    EruptDescriptorWriter& EruptDescriptorWriter::WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo)
    {
        assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = m_SetLayout.m_Bindings[binding];

        assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        m_Writes.push_back(write);
        return *this;
    }

    bool EruptDescriptorWriter::Build(VkDescriptorSet& set)
    {
        bool success = m_Pool.AllocateDescriptor(m_SetLayout.GetDescriptorSetLayout(), set);
        if (!success) 
        {
            return false;
        }
        Overwrite(set);
        return true;
    }

    void EruptDescriptorWriter::Overwrite(VkDescriptorSet& set)
    {
        for (auto& write : m_Writes)
        {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(m_Pool.m_Device.Device(), static_cast<uint32_t>(m_Writes.size()), m_Writes.data(), 0, nullptr);
    }
}