#include "core/FileIO.h"

#include "core/Log.h"

#include <iostream>
#include <fstream>
#include <vector>

namespace Erupt
{
	std::string FileIO::m_ResourcesPath = "";

	void FileIO::Init()
	{
		std::string currentDir = _SOLUTIONDIR;

		m_ResourcesPath = currentDir + "resources\\";
	}

	std::string FileIO::GetResourcesPath()
	{
		return m_ResourcesPath;
	}

	std::vector<char> FileIO::ReadFile(const std::string& filePath)
	{
		const std::string path = GetResourcesPath() + filePath;

		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			ERUPT_CORE_ERROR("BOOM!");
			throw std::runtime_error("Failed to open file: " + path);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	bool FileIO::WriteTextFile(const std::string& text, const std::string& filePath)
	{
		std::ofstream file;
		file.open(GetResourcesPath() + filePath);

		if (file.is_open())
		{
			file << text;
			file.close();
			return true;
		}

		printf("File with path: %s could not be found.\n", (GetResourcesPath() + filePath).c_str());

		return false;
	}

	bool FileIO::Exists(const std::string& filePath)
	{
		const std::string path = GetResourcesPath() + filePath;
		std::ifstream f(path.c_str());

		bool good = f.good();

		f.close();

		return good;
	}
}