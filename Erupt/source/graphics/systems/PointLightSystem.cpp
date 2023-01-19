#include "graphics/systems/PointLightSystem.h"

#include "core/FileIO.h"
#include "core/Log.h"

namespace Erupt
{
	PointLightSystem::PointLightSystem(EruptDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : m_EruptDevice(device)
	{
		CreatePipelineLayout(globalSetLayout);
		CreatePipeline(renderPass);
	}

	PointLightSystem::~PointLightSystem()
	{
		vkDestroyPipelineLayout(m_EruptDevice.Device(), m_PipelineLayout, nullptr);
	}

	void PointLightSystem::Init()
	{
		Log::Init();
		FileIO::Init();
	}

	void PointLightSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		/*VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);*/

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(m_EruptDevice.Device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			ERUPT_CORE_ERROR("Failed to create pipeline layout!");
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void PointLightSystem::CreatePipeline(VkRenderPass renderPass)
	{
		assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		EruptPipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_PipelineLayout;

		m_EruptPipeline = std::make_unique<EruptPipeline>(
			m_EruptDevice,
			"shaders/compiled/point_light.vert.spv",
			"shaders/compiled/point_light.frag.spv",
			pipelineConfig
			);
	}

	void PointLightSystem::Render(FrameInfo& frameInfo)
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

		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}
}