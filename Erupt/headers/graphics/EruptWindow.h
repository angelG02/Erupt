#pragma once

#define GLFW_INCLUDE_NONE
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

		inline VkExtent2D GetExtent() { return { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) }; }
		inline GLFWwindow* GetWindow() const { return m_Window; }

		inline bool WasWindowResized() { return m_FramebufferResized; }
		inline void ResetWindowResizedFlag() { m_FramebufferResized = false; }

	private:
		void Init();

		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

	private:
		int m_Width;
		int m_Height;
		bool m_FramebufferResized = false;

		std::string m_WindowName;

		GLFWwindow* m_Window;
	};

} // namespace Erupt