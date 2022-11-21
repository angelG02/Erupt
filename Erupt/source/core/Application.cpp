#include "core/Application.h"

#include "core/FileIO.h"

namespace Erupt
{
	Window Application::m_EruptWindow = Window( WINDOW_WIDTH, WINDOW_HEIGHT, "Henlo Vulkan!" );

	void Application::Init()
	{
		Log::Init();
		FileIO::Init();
		ERUPT_CORE_INFO("Initialized Erupt Engine!");
	}

	void Application::Run()
	{
		while (!m_EruptWindow.ShouldClose())
		{
			glfwPollEvents();
		}
	}
}