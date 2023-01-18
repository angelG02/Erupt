#pragma once

#include "EruptDevice.h"
#include "EruptBuffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace Erupt
{
	class Model
	{
	public:

		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

			bool operator==(const Vertex& other) const
			{
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void LoadModel(const std::string& filepath);
		};

		Model(EruptDevice& device, const Builder& builder);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		static std::unique_ptr<Model> CreateModelFromFile(EruptDevice& device, const std::string& filepath);

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

	private:
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffers(const std::vector<uint32_t>& indices);

	private:
		EruptDevice& m_Device;

		std::unique_ptr<EruptBuffer> m_VertexBuffer;
		uint32_t m_VertexCount;

		bool m_IsIndexed = false;
		std::unique_ptr<EruptBuffer> m_IndexBuffer;
		uint32_t m_IndexCount;
	};

}