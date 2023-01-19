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
		
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f }; // w is intensity
		glm::vec3 lightPosition{ -1.f };
		alignas(16) glm::vec4 lightColor{ 1.f }; // w is light intensity
	};

	Application::Application()
	{
		m_GlobalPool = EruptDescriptorPool::Builder(m_EruptDevice)
			.SetMaxSets(EruptSwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, EruptSwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();

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

		auto globalSetLayout = EruptDescriptorSetLayout::Builder(m_EruptDevice)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.Build();

		std::vector<VkDescriptorSet> globalDescriptorSets(EruptSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->DescriptorInfo();
			EruptDescriptorWriter(*globalSetLayout, *m_GlobalPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ m_EruptDevice, m_EruptRenderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout()};

		Camera camera{};
		camera.SetViewDirection(glm::vec3(0.f), glm::vec3(0.f, 0.f, 1.f));

		auto viewerEntity = Entity::CreateEntity();
		viewerEntity.m_Transform.translation.z = -2.5;
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
			camera.SetPerspectiveProjection(glm::radians(50.f), aspectRatio, 0.1f, 1000.f);

			if (auto commandBuffer = m_EruptRenderer.BeginFrame())
			{
				int frameIndex = m_EruptRenderer.GetFrameIndex();
				FrameInfo frameInfo{frameIndex, deltaTime, commandBuffer, camera, globalDescriptorSets[frameIndex]};

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
		std::shared_ptr<Model> flatVase = Model::CreateModelFromFile(m_EruptDevice, "models/flat_vase.obj");

		auto flatVaseEntity = Entity::CreateEntity();
		flatVaseEntity.m_Model = flatVase;
		flatVaseEntity.m_Transform.translation = { .5f, .5f, 0.f };
		flatVaseEntity.m_Transform.scale = { 3.f, 4.f, 3.f };

		m_Entities.emplace_back(std::move(flatVaseEntity));

		std::shared_ptr<Model> vase = Model::CreateModelFromFile(m_EruptDevice, "models/smooth_vase.obj");

		auto smoothVaseEntity = Entity::CreateEntity();
		smoothVaseEntity.m_Model = vase;
		smoothVaseEntity.m_Transform.translation = { -.5f, .5f, 0.f };
		smoothVaseEntity.m_Transform.scale = { 3.f, 4.f, 3.f };

		m_Entities.emplace_back(std::move(smoothVaseEntity));

		std::shared_ptr<Model> quad = Model::CreateModelFromFile(m_EruptDevice, "models/quad.obj");

		auto floor = Entity::CreateEntity();
		floor.m_Model = quad;
		floor.m_Transform.translation = { 0.f, .5f, 0.f };
		floor.m_Transform.scale = { 3.f, 1.f, 3.f };

		m_Entities.emplace_back(std::move(floor));
	}
}