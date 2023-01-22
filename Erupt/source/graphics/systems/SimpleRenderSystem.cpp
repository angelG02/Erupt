#include "graphics/systems/SimpleRenderSystem.h"

#include "core/FileIO.h"
#include "core/Log.h"

namespace Erupt
{
	struct SimplePushConstantData
	{
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	SimpleRenderSystem::SimpleRenderSystem(EruptDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : m_EruptDevice(device)
	{
		CreatePipelineLayout(globalSetLayout);
		CreatePipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(m_EruptDevice.Device(), m_PipelineLayout, nullptr);
	}

	void SimpleRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_EruptDevice.Device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			ERUPT_CORE_ERROR("Failed to create pipeline layout!");
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass)
	{
		assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		EruptPipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_PipelineLayout;

		m_EruptPipeline = std::make_unique<EruptPipeline>(
			m_EruptDevice,
			"shaders/compiled/simple_shader.vert.spv",
			"shaders/compiled/simple_shader.frag.spv",
			pipelineConfig
			);
	}

	void SimpleRenderSystem::RenderEntities(FrameInfo& frameInfo)
	{
		m_EruptPipeline->Bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_PipelineLayout,
			0, 
			1,
			&frameInfo.globalDescriptorSet,
			0, 
			nullptr
		);

		for (auto& kv : frameInfo.entities)
		{
			auto& entity = kv.second;

			// If the object does not have a model there is no need to render it
			if (entity.m_Model == nullptr) continue;

			if(entity.GetId() != 2)
				entity.m_Transform.rotation.y += 1.f * frameInfo.deltaTime;

			SimplePushConstantData push{};
			push.modelMatrix = entity.m_Transform.mat4();
			push.normalMatrix = entity.m_Transform.normalMatrix();

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				m_PipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);

			entity.m_Model->Bind(frameInfo.commandBuffer);
			entity.m_Model->Draw(frameInfo.commandBuffer);
		}
	}
}