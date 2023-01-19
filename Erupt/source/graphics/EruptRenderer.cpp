#include "graphics/EruptRenderer.h"

#include "core/FileIO.h"

#include <stdexcept>
#include <array>

namespace Erupt
{
	EruptRenderer::EruptRenderer(Window& window, EruptDevice& device)
		: m_EruptWindow(window), m_EruptDevice(device)
	{
		Init();
	}

	EruptRenderer::~EruptRenderer()
	{
		FreeCommandBuffers();
	}

	void EruptRenderer::Init()
	{
		RecreateSwapchain();
		CreateCommandBuffers();
	}

	VkCommandBuffer EruptRenderer::BeginFrame()
	{
		assert(!m_IsFrameStarted && "Cannot call BeginFrame while already in progress");

		auto result = m_EruptSwapChain->AcquireNextImage(&m_CurrentImageIndex);

		// Occurs when window is resized
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			ERUPT_CORE_ERROR("Failed to acquire swap chain image!");
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		m_IsFrameStarted = true;

		auto commandBuffer = GetCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			ERUPT_CORE_ERROR("Failed to begin recording command buffer!");
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void EruptRenderer::EndFrame()
	{
		assert(m_IsFrameStarted && "Cannot call EndFrame frame is not in progress!");

		auto commandBuffer = GetCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			ERUPT_CORE_ERROR("Failed to record command buffer!");
			throw std::runtime_error("Failed to record command buffer!");
		}

		auto result = m_EruptSwapChain->SubmitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_EruptWindow.WasWindowResized())
		{
			m_EruptWindow.ResetWindowResizedFlag();
			RecreateSwapchain();
		}
		else if (result != VK_SUCCESS)
		{
			ERUPT_CORE_ERROR("Failed to present swap chain image!");
			throw std::runtime_error("Failed to present swap chain image!");
		}

		m_IsFrameStarted = false;
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % EruptSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void EruptRenderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(m_IsFrameStarted && "Cannot begin render pass when frame is not in progress!");
		assert(commandBuffer == GetCurrentCommandBuffer() && "Cannot begin render pass on a command buffer from a different frame!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_EruptSwapChain->GetRenderPass();
		renderPassInfo.framebuffer = m_EruptSwapChain->GetFrameBuffer(m_CurrentImageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = m_EruptSwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.009f, 0.009f, 0.009f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_EruptSwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(m_EruptSwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0}, m_EruptSwapChain->GetSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}
	void EruptRenderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(m_IsFrameStarted && "Cannot end render pass when frame is not in progress!");
		assert(commandBuffer == GetCurrentCommandBuffer() && "Cannot end render pass on a command buffer from a different frame!");

		vkCmdEndRenderPass(commandBuffer);
	}

	void EruptRenderer::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(EruptSwapChain::MAX_FRAMES_IN_FLIGHT);

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
			std::shared_ptr<EruptSwapChain> oldSwapChain = std::move(m_EruptSwapChain);
			m_EruptSwapChain = std::make_unique<EruptSwapChain>(m_EruptDevice, extent, oldSwapChain);

			if (!oldSwapChain->CompareSwapFormats(*m_EruptSwapChain.get()))
			{
				ERUPT_CORE_ERROR("Swap chain image(or depth) format has changed!");
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		}
	}
}