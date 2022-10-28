#include "Application.h"

namespace Erupt
{
	Window Application::m_EruptWindow = Window( WINDOW_WIDTH, WINDOW_HEIGHT, "Henlo Vulkan!" );

	void Application::Init()
	{
		Log::Init();
		ERUPT_CORE_TRACE("Initialized Erupt Engine!");
		ERUPT_TRACE("Application number: {0}", 1);

	}

	void Application::Run()
	{
		while (!m_EruptWindow.ShouldClose())
		{
			glfwPollEvents();
		}
	}
}