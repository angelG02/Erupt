#include "core/Application.h"

#include "core/FileIO.h"

namespace Erupt
{
	Window Application::s_EruptWindow = Window( WINDOW_WIDTH, WINDOW_HEIGHT, "Henlo Vulkan!" );
	EruptPipeline Application::s_EruptPipeline = EruptPipeline("shaders/simple_shader.vert", "shaders/simple_shader.frag");

	void Application::Init()
	{
		Log::Init();
		FileIO::Init();
		s_EruptPipeline.Init();
		ERUPT_CORE_INFO("Initialized Erupt Engine!");
	}

	void Application::Run()
	{
		while (!s_EruptWindow.ShouldClose())
		{
			glfwPollEvents();
		}
	}
}