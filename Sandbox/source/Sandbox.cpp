#include "core/Application.h"

int main()
{
	Erupt::Engine.Init();

	try
	{
		Erupt::Engine.Run();
	}
	catch (const std::exception& e)
	{
		ERUPT_ERROR("{0}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}