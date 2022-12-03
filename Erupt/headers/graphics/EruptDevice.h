#pragma once

#include "EruptWindow.h"

// std lib headers
#include <string>
#include <vector>

namespace Erupt {

	struct SwapChainSupportDetails 
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct QueueFamilyIndices 
	{
		uint32_t graphicsFamily;
		uint32_t presentFamily;
		bool graphicsFamilyHasValue = false;
		bool presentFamilyHasValue = false;
		bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
	};

	class EruptDevice 
	{
	public:

#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

		EruptDevice(Window& window);
		~EruptDevice();

		void Init();

		// Not copyable or movable
		EruptDevice(const EruptDevice&) = delete;
		EruptDevice& operator=(const EruptDevice&) = delete;
		EruptDevice(EruptDevice&&) = delete;
		EruptDevice& operator=(EruptDevice&&) = delete;

		VkCommandPool GetCommandPool() { return m_CommandPool; }
		VkDevice Device() { return m_Device; }
		VkSurfaceKHR Surface() { return m_Surface; }
		VkQueue GraphicsQueue() { return m_GraphicsQueue; }
		VkQueue PresentQueue() { return m_PresentQueue; }

		SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(m_PhysicalDevice); }
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(m_PhysicalDevice); }
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		// Buffer Helper Functions
		void CreateBuffer(
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkBuffer& buffer,
			VkDeviceMemory& bufferMemory);

		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

		void CreateImageWithInfo(
			const VkImageCreateInfo& imageInfo,
			VkMemoryPropertyFlags properties,
			VkImage& image,
			VkDeviceMemory& imageMemory);

		VkPhysicalDeviceProperties properties;

	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateCommandPool();

		// helper functions
		bool IsDeviceSuitable(VkPhysicalDevice device);
		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void HasGflwRequiredInstanceExtensions();
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	private:
		VkInstance						m_Instance;
		VkDebugUtilsMessengerEXT		m_DebugMessenger;
		VkPhysicalDevice				m_PhysicalDevice = VK_NULL_HANDLE;
		Window&							m_Window;
		VkCommandPool					m_CommandPool;

		VkDevice						m_Device;
		VkSurfaceKHR					m_Surface;
		VkQueue							m_GraphicsQueue;
		VkQueue							m_PresentQueue;

		const std::vector<const char*>	m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*>	m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};

}  // namespace lve