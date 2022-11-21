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

		void Init();

	private:
		void CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath);

	private:
		std::string m_VertFilepath, m_FragFilepath;
	};

}