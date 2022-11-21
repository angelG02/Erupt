#pragma once

#include <string>
#include <vector>

namespace Erupt
{
	class EruptPipeline
	{
	public:
		EruptPipeline(const std::string& vertFilepath, const std::string& fragFilepath);

		~EruptPipeline() = default;

	private:
		static std::vector<char> ReadFile(const std::string& filepath);

		void CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
	};

}