#include "graphics/EruptRenderer.h"

#include "core/FileIO.h"

#include <stdexcept>
#include <array>

namespace Erupt
{
	struct SimplePushConstantData
	{
		glm::mat2 transform{ 1.f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	EruptRenderer::~EruptRenderer()
	{
		vkDestroyPipelineLayout(m_EruptDevice.Device(), m_PipelineLayout, nullptr);
	}

	void Erupt::EruptRenderer::Init()
	{
		m_EruptDevice.Init();
		
		CreatePipelineLayout();
		RecreateSwapchain();

		LoadModels();

		CreatePipeline();
		CreateCommandBuffers();
	}

	void EruptRenderer::DrawFrame()
	{
		uint32_t imageIndex;
		auto result = m_EruptSwapChain->AcquireNextImage(&imageIndex);

		// Occurs when window is resized
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			ERUPT_CORE_ERROR("Failed to acquire swap chain image!");
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		RecordCommandBuffer(imageIndex);

		result = m_EruptSwapChain->SubmitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_EruptWindow.WasWindowResized())
		{
			m_EruptWindow.ResetWindowResizedFlag();
			RecreateSwapchain();
			return;
		}

		if (result != VK_SUCCESS)
		{
			ERUPT_CORE_ERROR("Failed to present swap chain image!");
			throw std::runtime_error("Failed to present swap chain image!");
		}
	}

	void EruptRenderer::CreatePipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_EruptDevice.Device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			ERUPT_CORE_ERROR("Failed to create pipeline layout!");
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void EruptRenderer::CreatePipeline()
	{
		assert(m_EruptSwapChain != nullptr && "Cannot create pipeline before swapchain!");
		assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		EruptPipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = m_EruptSwapChain->GetRenderPass();
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
		m_CommandBuffers.resize(m_EruptSwapChain->ImageCount());

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
	}

	void EruptRenderer::RecordCommandBuffer(int imageIndex)
	{
		static int frame = 0;
		frame = (frame + 1) % 100;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
		{
			ERUPT_CORE_ERROR("Failed to begin recording command buffer!");
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_EruptSwapChain->GetRenderPass();
		renderPassInfo.framebuffer = m_EruptSwapChain->GetFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = m_EruptSwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.02f, 0.0f, 0.04f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_EruptSwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(m_EruptSwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0}, m_EruptSwapChain->GetSwapChainExtent() };
		vkCmdSetViewport(m_CommandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(m_CommandBuffers[imageIndex], 0, 1, &scissor);

		m_EruptPipeline->Bind(m_CommandBuffers[imageIndex]);
		m_Model->Bind(m_CommandBuffers[imageIndex]);

		for (int j = 0; j < 4; j++)
		{
			SimplePushConstantData push{};
			push.offset = { -0.5f + frame * 0.02f, -0.4f + j * 0.25f };
			push.color = { 0.0f, 0.0f, 0.2f + 0.2f * j };

			vkCmdPushConstants(
				m_CommandBuffers[imageIndex],
				m_PipelineLayout, 
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			
			m_Model->Draw(m_CommandBuffers[imageIndex]);
		}

		vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);
		if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS)
		{
			ERUPT_CORE_ERROR("Failed to record command buffer!");
			throw std::runtime_error("Failed to record command buffer!");
		}
	}

	void EruptRenderer::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_EruptDevice.Device(), m_EruptDevice.GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}

	void EruptRenderer::RecreateSwapchain()
	{
		auto extent = m_EruptWindow.GetExtent();
		
		// Ex: if minimized pause the program
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_EruptWindow.GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_EruptDevice.Device());

		if (m_EruptSwapChain == nullptr)
		{
			m_EruptSwapChain = std::make_unique<EruptSwapChain>(m_EruptDevice, extent);
		}
		else
		{
			m_EruptSwapChain = std::make_unique<EruptSwapChain>(m_EruptDevice, extent, std::move(m_EruptSwapChain));
			if (m_EruptSwapChain->ImageCount() != m_CommandBuffers.size())
			{
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}

		// TODO: If render passes are compatible do nothing else:
		CreatePipeline();
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