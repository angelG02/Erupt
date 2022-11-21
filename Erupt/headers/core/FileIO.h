#pragma once

#include <string>
#include <vector>

namespace Erupt
{
	class FileIO
	{
	public:
		/// <summary>
		/// Initializes the FileIO
		/// </summary>
		static void Init();

		/// <summary>
		/// Get the platform specific path to the Resources folder
		/// </summary>
		/// <returns>std::string containing the full path to Resources</returns>
		static std::string GetResourcesPath();

		/// <summary>
		/// Read from file
		/// </summary>
		/// <param name="std::string filePath:">Realtive to the project root folder file path to the file you would like to read from</param>
		/// <returns>vector<char> containing the contets read if operation is successful. If not throws an exception</returns>
		static std::vector<char> ReadFile(const std::string& filePath);

		/// <summary>
		/// Write to text file
		/// </summary>
		/// <param name="std::string text:">Text to write to the file</param>
		/// <param name="std::string filePath:">Realtive to the project root folder file path to the file you would like to read from</param>
		/// <returns>Returns true if sucessful and false otherwise</returns>
		static bool WriteTextFile(const std::string& text, const std::string& filePath);

		/// <summary>
		/// Checks if a file exists in the Resources folder
		/// </summary>
		/// <param name="std::string filePath:">Realtive to the project root folder file path to the file you would like to read from</param>
		/// <returns>Returns true if the file exists and false otherwise</returns>
		static bool Exists(const std::string& filePath);

	private:

		static std::string m_ResourcesPath;
	};
}