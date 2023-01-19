#pragma once

#include "EruptDevice.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace Erupt
{
    class EruptDescriptorSetLayout 
    {
    public:

        class Builder
        {
        public:
            Builder(EruptDevice& device) : m_Device{ device } {}

            Builder& AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);

            std::unique_ptr<EruptDescriptorSetLayout> Build() const;

        private:
            EruptDevice& m_Device;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings{};
        };

        EruptDescriptorSetLayout(EruptDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~EruptDescriptorSetLayout();

        EruptDescriptorSetLayout(const EruptDescriptorSetLayout&) = delete;
        EruptDescriptorSetLayout& operator=(const EruptDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }

    private:
        EruptDevice& m_Device;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings;

        friend class EruptDescriptorWriter;
    };

    class EruptDescriptorPool {
    public:
        class Builder {
        public:
            Builder(EruptDevice& device) : m_Device{ device } {}

            Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& SetMaxSets(uint32_t count);
            std::unique_ptr<EruptDescriptorPool> Build() const;

        private:
            EruptDevice& m_Device;
            std::vector<VkDescriptorPoolSize> m_PoolSizes{};
            uint32_t m_MaxSets = 1000;
            VkDescriptorPoolCreateFlags m_PoolFlags = 0;
        };

        EruptDescriptorPool(EruptDevice& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~EruptDescriptorPool();

        EruptDescriptorPool(const EruptDescriptorPool&) = delete;
        EruptDescriptorPool& operator=(const EruptDescriptorPool&) = delete;

        bool AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void ResetPool();

    private:
        EruptDevice& m_Device;
        VkDescriptorPool m_DescriptorPool;

        friend class EruptDescriptorWriter;
    };

    class EruptDescriptorWriter {
    public:
        EruptDescriptorWriter(EruptDescriptorSetLayout& setLayout, EruptDescriptorPool& pool);

        EruptDescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        EruptDescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool Build(VkDescriptorSet& set);
        void Overwrite(VkDescriptorSet& set);

    private:
        EruptDescriptorSetLayout& m_SetLayout;
        EruptDescriptorPool& m_Pool;
        std::vector<VkWriteDescriptorSet> m_Writes;
    };
}