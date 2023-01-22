#pragma once

#include "graphics/EruptPipeline.h"
#include "graphics/EruptFrameInfo.h"
#include "graphics/EruptDescriptors.h"

#include "ECS/Entity.h"
#include "core/Camera.h"

namespace Erupt
{
	class EditorSystem
	{
	public:
		EditorSystem(EruptDevice& device, Window& window, VkRenderPass renderPass, size_t imageCount);
		~EditorSystem();

		void Update(FrameInfo& frameInfo, GlobalUbo& ubo);
		void Render(FrameInfo& frameInfo);

	private:
		/*void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);*/

	private:
		EruptDevice&							m_EruptDevice;
		std::unique_ptr<EruptDescriptorPool>	m_ImguiDescriporPool;
		/*std::unique_ptr<EruptPipeline>			m_EruptPipeline;
		VkPipelineLayout						m_PipelineLayout;*/
	};

} // namespace Erupt