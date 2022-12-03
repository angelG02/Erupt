#pragma once

#include "graphics/EruptDevice.h"

#include <string>
#include <vector>

namespace Erupt
{
	struct PipelineConfigInfo
	{
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class EruptPipeline
	{
	public:
		EruptPipeline(
			EruptDevice& device, 
			const std::string& vertFilepath, 
			const std::string& fragFilepath, 
			const PipelineConfigInfo& configInfo);

		~EruptPipeline();

		EruptPipeline(const EruptPipeline&) = delete;
		EruptPipeline& operator=(const EruptPipeline&) = delete;

		void Init();
		void Bind(VkCommandBuffer commandBuffer);

		static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo, uint32_t width, uint32_t height);

	private:
		void CreateGraphicsPipeline(
			const std::string& vertFilepath, 
			const std::string& fragFilepath, 
			const PipelineConfigInfo& configInfo);

		void CreateShaderModule(const std::vector<char>& shaderCode, VkShaderModule* shaderModule);

	private:
		std::string m_VertFilepath, m_FragFilepath;

		PipelineConfigInfo m_ConfigInfo;
		EruptDevice& m_Device;
		VkPipeline m_GraphicsPipeline;
		VkShaderModule m_VertShaderModule;
		VkShaderModule m_FragShaderModule;
	};
}