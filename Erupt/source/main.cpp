#include "Application.h"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
    Erupt::Application app{};

	try
	{
		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}