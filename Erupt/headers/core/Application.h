#pragma once

#include "graphics/EruptRenderer.h"

#include "ECS/Entity.h"

namespace Erupt
{
	static constexpr int WINDOW_WIDTH = 800;
	static constexpr int WINDOW_HEIGHT = 600;

	class Application
	{
	public:
		Application();
		~Application();

		static void Init();

		void Run();

	private:
		void LoadEntities();

	private:
		Window							m_EruptWindow{ WINDOW_WIDTH, WINDOW_HEIGHT, "Henlo Vulkan!" };
		EruptDevice						m_EruptDevice{ m_EruptWindow };

		EruptRenderer					m_EruptRenderer{ m_EruptWindow, m_EruptDevice };

		std::vector<Entity>				m_Entities;
	};

} // namespace Erupt
