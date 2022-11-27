#include "core/Application.h"

#include "core/FileIO.h"

namespace Erupt
{
	Application& Engine = *(new Application()); // This line creates the Engine

	void Application::Init()
	{
		Log::Init();
		FileIO::Init();

		m_EruptRenderer.Init();

		ERUPT_CORE_INFO("Initialized Erupt Engine!");
	}

	void Application::Run()
	{
		while (!m_EruptRenderer.GetWindow().ShouldClose())
		{
			glfwPollEvents();
		}
	}
}