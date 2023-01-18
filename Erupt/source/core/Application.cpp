#include "core/Application.h"

#include "core/FileIO.h"
#include "core/Camera.h"
#include "core/Input.h"

#include "graphics/EruptBuffer.h"
#include "graphics/systems/SimpleRenderSystem.h"

#include <glm/gtc/constants.hpp>
#include <chrono>

namespace Erupt
{
	struct GlobalUbo
	{
		glm::mat4 viewProjection{ 1.f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
	};

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
		// Creating 2 UBOs for each frame in flight so that there is no need for idling between frames
		std::vector<std::unique_ptr<EruptBuffer>> uboBuffers(EruptSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<EruptBuffer>(
				m_EruptDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->Map();
		}

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
				int frameIndex = m_EruptRenderer.GetFrameIndex();
				FrameInfo frameInfo{frameIndex, deltaTime, commandBuffer, camera};

				// Update
				GlobalUbo ubo{};
				ubo.viewProjection = camera.GetProjection() * camera.GetView();
				uboBuffers[frameIndex]->WriteToBuffer(&ubo);
				uboBuffers[frameIndex]->Flush();

				// Render

				// begin offscreen shadow pass
				// render shadow casting objects	<--- Why render pass and frame are not combined
				// end offscreen shadow pass

				m_EruptRenderer.BeginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.RenderEntities(frameInfo, m_Entities);
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