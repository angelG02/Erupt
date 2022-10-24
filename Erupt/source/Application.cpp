#include "Application.h"

namespace Erupt
{
	Window Application::m_EruptWindow = Window( WINDOW_WIDTH, WINDOW_HEIGHT, "Henlo Vulkan!" );

	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (!m_EruptWindow.ShouldClose())
		{
			glfwPollEvents();
		}
	}
}