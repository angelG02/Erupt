#include "core/FileIO.h"

#include <iostream>
#include <fstream>
#include <vector>

namespace Erupt
{
	std::string FileIO::m_ResourcesPath = "";

	void FileIO::Init()
	{
		std::string currentDir = _SOLUTIONDIR;

		m_ResourcesPath = currentDir + "\\resources\\";
	}

	std::string FileIO::GetResourcesPath()
	{
		return m_ResourcesPath;
	}

	std::string	FileIO::ReadTextFile(const std::string& filePath)
	{
		const std::string path = GetResourcesPath() + filePath;

		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			printf("ERROR: file %s with path %s not found!", filePath.c_str(), path.c_str());
			return std::string();
		}

		file.seekg(0, std::ios::end);
		const size_t size = file.tellg();
		std::string buffer(size, '\0');
		file.seekg(0);
		file.read(&buffer[0], size);

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