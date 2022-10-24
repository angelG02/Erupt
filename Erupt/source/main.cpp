#include "Application.h"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {

	try
	{
		Erupt::Application::Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}