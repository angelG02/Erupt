#include "core/Input.h"

#include <limits>

namespace Erupt
{
	void Erupt::Input::MoveInPlaneXZ(GLFWwindow* window, float dt, Entity& entity)
	{
		glm::vec3 rotate{ 0.f };
		if (glfwGetKey(window, m_Keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
		if (glfwGetKey(window, m_Keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
		if (glfwGetKey(window, m_Keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
		if (glfwGetKey(window, m_Keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		{
			entity.m_Transform.rotation += m_LookSpeed * dt * glm::normalize(rotate);
		}

		// limit pitch values between +/- 85ish degrees
		entity.m_Transform.rotation.x = glm::clamp(entity.m_Transform.rotation.x, -1.5f, 1.5f);
		entity.m_Transform.rotation.y = glm::mod(entity.m_Transform.rotation.y, glm::two_pi<float>());

		float yaw = entity.m_Transform.rotation.y;
		const glm::vec3 forwarDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwarDir.z, 0.f, -forwarDir.x };
		const glm::vec3 upDir{ 0.f, -1.f, 0.f };

		glm::vec3 moveDir{ 0.f };
		if (glfwGetKey(window, m_Keys.moveForward) == GLFW_PRESS) moveDir += forwarDir;
		if (glfwGetKey(window, m_Keys.moveBackward) == GLFW_PRESS) moveDir -= forwarDir;
		if (glfwGetKey(window, m_Keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window, m_Keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(window, m_Keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window, m_Keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
		{
			entity.m_Transform.translation += m_MoveSpeed * dt * glm::normalize(moveDir);
		}
	}

}