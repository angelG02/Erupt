#pragma once

#include "graphics/EruptWindow.h"

namespace Erupt
{
	static constexpr int WINDOW_WIDTH = 800;
	static constexpr int WINDOW_HEIGHT = 600;

	class Application
	{
	public:
		Application();
		~Application();

		void Run();
	public:


	private:
		Window m_EruptWindow{ WINDOW_WIDTH, WINDOW_HEIGHT, "Henlo Vulkan!" };
	};

} // namespace Erupt
