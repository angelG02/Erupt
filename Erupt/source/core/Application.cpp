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

	void Application::LoadEntities()
	{
		std::shared_ptr<Model> model = Model::CreateModelFromFile(m_EruptDevice, "models/flat_vase.obj");

		auto entity = Entity::CreateEntity();
		entity.m_Model = model;
		entity.m_Transform.translation = { .0f, .5f, 2.5f };
		entity.m_Transform.scale = { 3.f, 4.f, 3.f };

		m_Entities.emplace_back(std::move(entity));
	}
}