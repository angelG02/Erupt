#include "graphics/EruptPipeline.h"

#include "core/FileIO.h"
#include "core/Log.h"

#include <fstream>
#include <stdexcept>

namespace Erupt
{
	EruptPipeline::EruptPipeline(const std::string& vertFilepath, const std::string& fragFilepath)
		: m_VertFilepath(vertFilepath), m_FragFilepath(fragFilepath)
	{
		
	}

	void EruptPipeline::Init()
	{
		CreateGraphicsPipeline(m_VertFilepath, m_FragFilepath);
	}

	void EruptPipeline::CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath)
	{
		auto vertCode = FileIO::ReadFile(vertFilepath);
		auto fragCode = FileIO::ReadFile(fragFilepath);
	}
}