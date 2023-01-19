#pragma once

#include "graphics/EruptPipeline.h"
#include "graphics/EruptFrameInfo.h"

#include "ECS/Entity.h"
#include "core/Camera.h"

namespace Erupt
{
	class PointLightSystem
	{
	public:
		PointLightSystem(EruptDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		static void Init();

		void Render(FrameInfo& frameInfo);

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

	private:
		EruptDevice& m_EruptDevice;

		std::unique_ptr<EruptPipeline>	m_EruptPipeline;
		VkPipelineLayout				m_PipelineLayout;
	};

} // namespace Erupt
