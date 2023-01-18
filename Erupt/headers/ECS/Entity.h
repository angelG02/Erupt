#pragma once

#include "graphics/Model.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace Erupt
{

	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation;

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
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
		TransformComponent m_Transform{};

	private:
		id_t m_Id;
	};
}