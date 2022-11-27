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

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		bool ShouldClose();

		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

		VkExtent2D GetExtent() { return { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) }; }

	private:
		void Init();

	private:
		const int m_Width;
		const int m_Height;

		std::string m_WindowName;

		GLFWwindow* m_Window;
	};

} // namespace Erupt