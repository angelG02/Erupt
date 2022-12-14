#include "core/Application.h"

int main()
{
	//This is cursed
	Erupt::Application::Init();
	Erupt::Application engine = Erupt::Application();

	try
	{
		engine.Run();
	}
	catch (const std::exception& e)
	{
		ERUPT_ERROR("{0}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}