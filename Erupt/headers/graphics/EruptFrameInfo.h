#pragma once

#include "core/Camera.h"
#include "ECS/Entity.h"

// lib
#include <vulkan/vulkan.h>

namespace Erupt
{
	constexpr auto MAX_LIGHTS = 10;

	struct PointLight
	{
		glm::vec4 position{}; // ignore w
		glm::vec4 color{}; // w is intensity
	};

	struct GlobalUbo
	{
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };

		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f }; // w is intensity
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};

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