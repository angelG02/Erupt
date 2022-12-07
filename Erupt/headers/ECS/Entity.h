#pragma once

#include "graphics/Model.h"

#include <memory>

namespace Erupt
{

	struct Transform2dComponent
	{
		glm::vec2 translation{};
		glm::vec2 scale{ 1.f, 1.f };
		float rotation;

		inline glm::mat2 mat2() 
		{ 
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{ {c, s}, {-s, c} };

			glm::mat2 scaleMat{ {scale.x, 0.f}, {0.f, scale.y} };
			return rotMatrix * scaleMat;
		}
	};

	class Entity
	{
	public:
		using id_t = unsigned int;

		inline static Entity CreateEntity()
		{
			static id_t currentId = 0;
			return Entity(currentId++);
		}

		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;
		Entity(Entity&&) = default;
		Entity& operator=(Entity&&) = default;

		inline id_t GetId() const { return m_Id; }

	private:
		inline Entity(id_t entityId) : m_Id(entityId) {}

	public:
		std::shared_ptr<Model> m_Model{};
		glm::vec3 m_Color{};
		Transform2dComponent m_Transform2d{};

	private:
		id_t m_Id;
	};
}