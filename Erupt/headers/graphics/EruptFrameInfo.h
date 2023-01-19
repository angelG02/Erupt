#pragma once

#include "core/Camera.h"
#include "ECS/Entity.h"

// lib
#include <vulkan/vulkan.h>

namespace Erupt
{
	// Struct for storing frame data that will be reused throghout the whole application
	struct FrameInfo
	{
		int frameIndex;
		float deltaTime;
		VkCommandBuffer commandBuffer;
		Camera& camera;
		VkDescriptorSet globalDescriptorSet;

		Entity::Map& entities;
	};
}