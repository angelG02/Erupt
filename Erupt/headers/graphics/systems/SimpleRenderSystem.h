#pragma once

#include "graphics/EruptPipeline.h"
#include "graphics/EruptFrameInfo.h"

#include "ECS/Entity.h"
#include "core/Camera.h"

namespace Erupt
{
	class SimpleRenderSystem
	{
	public:
		SimpleRenderSystem(EruptDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		static void Init();
		
		void RenderEntities(FrameInfo& frameInfo, std::vector<Entity>& entities);

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

	private:
		EruptDevice&					m_EruptDevice;

		std::unique_ptr<EruptPipeline>	m_EruptPipeline;
		VkPipelineLayout				m_PipelineLayout;
	};

} // namespace Erupt
