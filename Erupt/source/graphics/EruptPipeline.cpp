#include "graphics/EruptPipeline.h"

#include "Log.h"

#include <fstream>
#include <stdexcept>

namespace Erupt
{
	std::vector<char> EruptPipeline::ReadFile(const std::string& filepath)
	{
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			ERUPT_CORE_ERROR("BOOM!");
			throw std::runtime_error("Failed to open file: " + filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	EruptPipeline::EruptPipeline(const std::string& vertFilepath, const std::string& fragFilepath)
	{
		CreateGraphicsPipeline(vertFilepath, fragFilepath);
	}

	void EruptPipeline::CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath)
	{
		auto vertCode = ReadFile(vertFilepath);
		auto fragCode = ReadFile(fragFilepath);

		ERUPT_CORE_INFO("Vertex shader code size: {0}", vertCode.size());
		ERUPT_CORE_INFO("Fragment shader code size: {0}", fragCode.size());
	}
}