#include "core/Application.h"

#include "core/FileIO.h"
#include "core/Camera.h"
#include "core/Input.h"

#include "graphics/systems/SimpleRenderSystem.h"

#include <glm/gtc/constants.hpp>
#include <chrono>

namespace Erupt
{
	Application::Application()
	{
		LoadEntities();
	}

	Application::~Application()
	{
	}

	void Application::Init()
	{
		Log::Init();
		FileIO::Init();
		ERUPT_CORE_INFO("Initializing Erupt Engine!");
	}

	void Application::Run()
	{
		SimpleRenderSystem simpleRenderSystem{ m_EruptDevice, m_EruptRenderer.GetSwapChainRenderPass() };

		Camera camera{};
		camera.SetViewDirection(glm::vec3(0.f), glm::vec3(0.f, 0.f, 1.f));

		auto viewerEntity = Entity::CreateEntity();
		Input cameraControler{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!m_EruptWindow.ShouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			// Max frame time??? For when polling events takes longer?
			//deltaTime = glm::min(deltaTime, MAX_FRAME_TIME);

			cameraControler.MoveInPlaneXZ(m_EruptWindow.GetWindow(), deltaTime, viewerEntity);
			camera.SetViewYXZ(viewerEntity.m_Transform.translation, viewerEntity.m_Transform.rotation);
			
			float aspectRatio = m_EruptRenderer.GetAspectRatio();
			camera.SetPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 10.f);

			if (auto commandBuffer = m_EruptRenderer.BeginFrame())
			{
				// begin offscreen shadow pass
				// render shadow casting objects	<--- Why render pass and frame are not combined
				// end offscreen shadow pass

				m_EruptRenderer.BeginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.RenderEntities(commandBuffer, m_Entities, camera);
				m_EruptRenderer.EndSwapChainRenderPass(commandBuffer);
				m_EruptRenderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_EruptDevice.Device());
	}

	// Temp load cube function
	std::unique_ptr<Model> createCubeModel(EruptDevice& device, glm::vec3 offset) 
	{
		Model::Builder modelBuilder{};

		modelBuilder.vertices = 
		{
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},

		};

		for (auto& v : modelBuilder.vertices) {
			v.position += offset;
		}

		modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
						  12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

		return std::make_unique<Model>(device, modelBuilder);
	}

	void Application::LoadEntities()
	{
		std::shared_ptr<Model> model = createCubeModel(m_EruptDevice, { .0f, .0f, .0f });

		auto cube = Entity::CreateEntity();
		cube.m_Model = model;
		cube.m_Transform.translation = { .0f, .0f, 2.5f };
		cube.m_Transform.scale = { .5f, .5f, .5f };

		m_Entities.emplace_back(std::move(cube));
	}
}