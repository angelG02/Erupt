#pragma once

#include "graphics/EruptWindow.h"
#include "graphics/EruptPipeline.h"
#include "graphics/EruptSwapChain.h"

#include "graphics/Model.h"

#include "core/Log.h"

#include <memory>
#include <vector>

namespace Erupt
{
	static constexpr int WINDOW_WIDTH = 800;
	static constexpr int WINDOW_HEIGHT = 600;

	class EruptRenderer
	{
	public:
		EruptRenderer() = default;
		~EruptRenderer();

		EruptRenderer(const EruptRenderer&) = delete;
		EruptRenderer& operator=(const EruptRenderer&) = delete;

		void Init();
		void DrawFrame();

		Window& GetWindow() { return m_EruptWindow; }
		EruptDevice& GetDevice() { return m_EruptDevice; }

	private:
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();

		void LoadModels();

	private:
		Window							m_EruptWindow{ WINDOW_WIDTH, WINDOW_HEIGHT, "Henlo Vulkan!" };
		EruptDevice						m_EruptDevice{ m_EruptWindow };
		EruptSwapChain					m_EruptSwapChain{ m_EruptDevice, m_EruptWindow.GetExtent() };

		std::unique_ptr<EruptPipeline>	m_EruptPipeline;
		VkPipelineLayout				m_PipelineLayout;

		std::vector<VkCommandBuffer>	m_CommandBuffers;

		std::unique_ptr<Model>			m_Model;
	};

}