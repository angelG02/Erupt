#pragma once

#include "graphics/EruptWindow.h"

#include "Log.h"

namespace Erupt
{
	static constexpr int WINDOW_WIDTH = 800;
	static constexpr int WINDOW_HEIGHT = 600;

	class Application
	{
	public:
		static void Init();

		static void Run();
	public:


	private:
		static Window m_EruptWindow;
	};

} // namespace Erupt
