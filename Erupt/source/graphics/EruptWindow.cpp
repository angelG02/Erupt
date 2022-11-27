#include "graphics/EruptWindow.h"

#include "core/Log.h"

#include <stdexcept>

namespace Erupt
{
	Window::Window(int width, int height, const std::string& name) : m_Width(width), m_Height(height), m_WindowName(name)
	{
		Init();
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Window::Init()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_Window = glfwCreateWindow(m_Width, m_Height, m_WindowName.c_str(), nullptr, nullptr);
	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(m_Window);
	}

	void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS)
		{
			ERUPT_CORE_ERROR("Failed to create window surface!");
			throw std::runtime_error("failed to create window surface");
		}
	}

} // namespace Erupt