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

	// Temp load cube function
	std::unique_ptr<Model> createCubeModel(EruptDevice& device, glm::vec3 offset) {
		std::vector<Model::Vertex> vertices{

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<Model>(device, vertices);
	}

	void Application::LoadEntities()
	{
		std::shared_ptr<Model> model = createCubeModel(m_EruptDevice, { .0f, .0f, .0f });

		auto cube = Entity::CreateEntity();
		cube.m_Model = model;
		cube.m_Transform.translation = { .0f, .0f, .5f };
		cube.m_Transform.scale = { .5f, .5f, .5f };

		m_Entities.emplace_back(std::move(cube));
	}
}