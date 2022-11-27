#include "graphics/EruptRenderer.h"

#include "core/FileIO.h"

#include <stdexcept>

namespace Erupt
{
	EruptRenderer::~EruptRenderer()
	{
		vkDestroyPipelineLayout(m_EruptDevice.Device(), m_PipelineLayout, nullptr);
	}

	void Erupt::EruptRenderer::Init()
	{
		m_EruptDevice.Init();
		m_EruptSwapChain.Init();

		CreatePipelineLayout();
		CreatePipeline();
		CreateCommandBuffers();
	}

	void EruptRenderer::DrawFrame()
	{

	}

	void EruptRenderer::CreatePipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(m_EruptDevice.Device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			ERUPT_CORE_ERROR("Failed to create pipeline layout!");
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void EruptRenderer::CreatePipeline()
	{
		PipelineConfigInfo pipelineConfig{};
		EruptPipeline::DefaultPipelineConfigInfo(pipelineConfig, m_EruptSwapChain.Width(), m_EruptSwapChain.Height());
		pipelineConfig.renderPass = m_EruptSwapChain.GetRenderPass();
		pipelineConfig.pipelineLayout = m_PipelineLayout;

		m_EruptPipeline = std::make_unique<EruptPipeline>(
			m_EruptDevice,
			"shaders/compiled/simple_shader.vert.spv",
			"shaders/compiled/simple_shader.frag.spv",
			pipelineConfig
			);
		m_EruptPipeline->Init();
	}

	void EruptRenderer::CreateCommandBuffers()
	{

	}
}