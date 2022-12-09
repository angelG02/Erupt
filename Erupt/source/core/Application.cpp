#include "core/Application.h"

#include "core/FileIO.h"
#include "graphics/systems/SimpleRenderSystem.h"

#include <glm/gtc/constants.hpp>

namespace Erupt
{
	Application::Application()
	{
		LoadEntities();
		
		ERUPT_CORE_INFO("Initialized Erupt Engine!");
	}

	Application::~Application()
	{
	}

	void Application::Init()
	{
		Log::Init();
		FileIO::Init();
	}

	void Application::Run()
	{
		SimpleRenderSystem simpleRenderSystem{ m_EruptDevice, m_EruptRenderer.GetSwapChainRenderPass() };
		while (!m_EruptWindow.ShouldClose())
		{
			glfwPollEvents();

			if (auto commandBuffer = m_EruptRenderer.BeginFrame())
			{
				// begin offscreen shadow pass
				// render shadow casting objects	<--- Why render pass and frame are not combined
				// end offscreen shadow pass

				m_EruptRenderer.BeginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.RenderEntities(commandBuffer, m_Entities);
				m_EruptRenderer.EndSwapChainRenderPass(commandBuffer);
				m_EruptRenderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_EruptDevice.Device());
	}

	void Application::LoadEntities()
	{
		std::vector<Model::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		auto model = std::make_shared<Model>(m_EruptDevice, vertices);

		auto triangle = Entity::CreateEntity();
		triangle.m_Model = model;
		triangle.m_Color = { .1f, .8f, .1f };
		triangle.m_Transform2d.translation.x = .2f;
		triangle.m_Transform2d.scale = { 2.f, .5f };
		triangle.m_Transform2d.rotation = .25f * glm::two_pi<float>();

		m_Entities.emplace_back(std::move(triangle));
	}
}