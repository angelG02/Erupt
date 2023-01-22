#include "graphics/systems/EditorSystem.h"

#include "core/Log.h"
#include "graphics/EruptSwapChain.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "vulkan/vulkan.h"

// std headers
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>


// local callback functions
static void debugCallback(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}

Erupt::EditorSystem::EditorSystem(EruptDevice& device, Window& window, VkRenderPass renderPass, size_t imageCount)
	: m_EruptDevice(device)
{
	m_ImguiDescriporPool =  EruptDescriptorPool::Builder(m_EruptDevice)
		.SetMaxSets(EruptSwapChain::MAX_FRAMES_IN_FLIGHT)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000)
		.Build();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	auto ctx = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForVulkan(window.GetWindow(), true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = m_EruptDevice.GetInstance();
	init_info.PhysicalDevice = m_EruptDevice.PhysicalDevice();
	init_info.Device = m_EruptDevice.Device();
	init_info.QueueFamily = 0;
	init_info.Queue = m_EruptDevice.GraphicsQueue();
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = m_ImguiDescriporPool->GetPool();
	init_info.Allocator = nullptr;
	init_info.MinImageCount = 2;
	init_info.ImageCount = imageCount;
	init_info.CheckVkResultFn = debugCallback;
	ImGui_ImplVulkan_Init(&init_info, renderPass);

	// Upload Fonts
	{
		auto err = vkResetCommandPool(m_EruptDevice.Device(), m_EruptDevice.GetCommandPool(), 0);
		debugCallback(err);
		auto commandBuffer = m_EruptDevice.BeginSingleTimeCommands();

		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

		m_EruptDevice.EndSingleTimeCommands(commandBuffer);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
	ImGui::SetCurrentContext(ctx);
}

Erupt::EditorSystem::~EditorSystem()
{
	// Resources to destroy when the program ends
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Erupt::EditorSystem::Update(FrameInfo& frameInfo, GlobalUbo& ubo)
{
}

void Erupt::EditorSystem::Render(FrameInfo& frameInfo)
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();

	// Rendering
	ImGui::Render();
	ImDrawData* main_draw_data = ImGui::GetDrawData();

	// Record dear imgui primitives into command buffer
	ImGui_ImplVulkan_RenderDrawData(main_draw_data, frameInfo.commandBuffer);

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

//void Erupt::EditorSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
//{
//}
//
//void Erupt::EditorSystem::CreatePipeline(VkRenderPass renderPass)
//{
//}
