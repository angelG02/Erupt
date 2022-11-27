#pragma once

#include "graphics/EruptRenderer.h"

namespace Erupt
{
	class Application
	{
	public:
		void Init();

		void Run();

	private:
		EruptRenderer m_EruptRenderer;
	};

	extern Application& Engine;

} // namespace Erupt
