#pragma once

#include "graphics/EruptWindow.h"
#include "graphics/EruptPipeline.h"

#include "Log.h"

namespace Erupt
{
	static constexpr int WINDOW_WIDTH = 800;
	static constexpr int WINDOW_HEIGHT = 600;

	class Application
	{
	public:
		void Init();

		void Run();
	public:


	private:
		Window m_EruptWindow { WINDOW_WIDTH, WINDOW_HEIGHT, "Henlo Vulkan!" };
		EruptPipeline m_EruptPipeline { "shaders/compiled/simple_shader.vert.spv", "shaders/compiled/simple_shader.frag.spv" };
	};

	extern Application& Engine;

} // namespace Erupt
