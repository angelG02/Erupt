#pragma once

#include "graphics/EruptWindow.h"
#include "graphics/EruptSwapChain.h"

#include "core/Log.h"

#include <memory>
#include <vector>
#include <cassert>

namespace Erupt
{

	class EruptRenderer
	{
	public:
		EruptRenderer(Window& window, EruptDevice& device);
		~EruptRenderer();

		EruptRenderer(const EruptRenderer&) = delete;
		EruptRenderer& operator=(const EruptRenderer&) = delete;

		void Init();

		VkCommandBuffer BeginFrame();
		void EndFrame();

		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

		inline bool IsFrameInProgress() const { return m_IsFrameStarted; }

		inline VkRenderPass GetSwapChainRenderPass() const { return m_EruptSwapChain->GetRenderPass(); }

		inline VkCommandBuffer GetCurrentCommandBuffer() const
		{
			assert(m_IsFrameStarted && "Cannot get command buffer when frame not in progress");
			return m_CommandBuffers[m_CurrentFrameIndex];
		}

		inline int GetFrameIndex() const 
		{ 
			assert(m_IsFrameStarted && "Cannot get current frame incex when frame not in progress");
			return m_CurrentFrameIndex;
		};

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();

		void RecreateSwapchain();

	private:
		Window&							m_EruptWindow;
		EruptDevice&					m_EruptDevice;
		std::unique_ptr<EruptSwapChain>	m_EruptSwapChain;

		std::vector<VkCommandBuffer>	m_CommandBuffers;

		uint32_t m_CurrentImageIndex = 0;
		int m_CurrentFrameIndex = 0;
		bool m_IsFrameStarted = false;
	};

}