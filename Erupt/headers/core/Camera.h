#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Erupt
{
	class Camera
	{
	public:
		void SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
		void SetPerspectiveProjection(float fovY, float aspectRatio, float near, float far);

		void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3(0.f, -1.f, 0.f));
		void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3(0.f, -1.f, 0.f));
		void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);

		inline const glm::mat4& GetProjection() const { return m_Projection; }
		inline const glm::mat4& GetView() const { return m_View; }

	private:
		glm::mat4 m_Projection{ 1.f };
		glm::mat4 m_View{ 1.f };
	};

}