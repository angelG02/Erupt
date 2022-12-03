#include "graphics/EruptRenderer.h"

#include "core/FileIO.h"

#include <stdexcept>
#include <array>

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

		LoadModels();

		CreatePipelineLayout();
		CreatePipeline();
		CreateCommandBuffers();
	}

	void EruptRenderer::DrawFrame()
	{
		uint32_t imageIndex;
		auto result = m_EruptSwapChain.AcquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			ERUPT_CORE_ERROR("Failed to acquire swap chain image!");
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		result = m_EruptSwapChain.SubmitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS)
		{
			ERUPT_CORE_ERROR("Failed to present swap chain image!");
			throw std::runtime_error("Failed to present swap chain image!");
		}
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
		m_CommandBuffers.resize(m_EruptSwapChain.ImageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_EruptDevice.GetCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_EruptDevice.Device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		{
			ERUPT_CORE_ERROR("Failed to create command buffers!");
			throw std::runtime_error("Failed to create command buffers!");
		}

		for (int i = 0; i < m_CommandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				ERUPT_CORE_ERROR("Failed to begin recording command buffer!");
				throw std::runtime_error("Failed to begin recording command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_EruptSwapChain.GetRenderPass();
			renderPassInfo.framebuffer = m_EruptSwapChain.GetFrameBuffer(i);

			renderPassInfo.renderArea.offset = { 0,0 };
			renderPassInfo.renderArea.extent = m_EruptSwapChain.GetSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.02f, 0.0f, 0.04f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			m_EruptPipeline->Bind(m_CommandBuffers[i]);
			m_Model->Bind(m_CommandBuffers[i]);
			m_Model->Draw(m_CommandBuffers[i]);

			vkCmdEndRenderPass(m_CommandBuffers[i]);
			if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS)
			{
				ERUPT_CORE_ERROR("Failed to record command buffer!");
				throw std::runtime_error("Failed to record command buffer!");
			}
		}
	}

	void EruptRenderer::LoadModels()
	{
		std::vector<Model::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		m_Model = std::make_unique<Model>(m_EruptDevice, vertices);
	}
}