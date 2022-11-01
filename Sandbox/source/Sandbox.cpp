#include "Application.h"

int main()
{
	try
	{
		Erupt::Application::Init();
		Erupt::Application::Run();
	}
	catch (const std::exception& e)
	{
		ERUPT_ERROR("{0}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}