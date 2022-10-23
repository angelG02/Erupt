#include "Application.h"

namespace Erupt
{
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