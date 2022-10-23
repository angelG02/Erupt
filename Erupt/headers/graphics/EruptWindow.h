#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace Erupt
{
	class Window
	{
	public:
		Window(int width, int height, const std::string& name);
		~Window();

		bool ShouldClose();

	private:
		void Initialize();

	private:
		const int m_Width;
		const int m_Height;

		std::string m_WindowName;

		GLFWwindow* m_Window;
	};

} // namespace Erupt