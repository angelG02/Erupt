#include "core/Camera.h"

#include <cassert>
#include <limits>

namespace Erupt
{
	void Camera::SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far)
	{
		m_Projection = glm::mat4{ 1.0f };
		m_Projection[0][0] = 2.f / (right - left);
		m_Projection[1][1] = 2.f / (bottom - top);
		m_Projection[2][2] = 1.f / (far - near);
		m_Projection[3][0] = -(right + left) / (right - left);
		m_Projection[3][1] = -(bottom + top) / (bottom - top);
		m_Projection[3][2] = -near / (far - near);
	}

	void Camera::SetPerspectiveProjection(float fovY, float aspectRatio, float near, float far)
	{
		assert(glm::abs(aspectRatio - std::numeric_limits<float>::epsilon()) > 0.0f);
		const float tanHalfFovy = tan(fovY / 2.f);
		m_Projection = glm::mat4{ 0.0f };
		m_Projection[0][0] = 1.f / (aspectRatio * tanHalfFovy);
		m_Projection[1][1] = 1.f / (tanHalfFovy);
		m_Projection[2][2] = far / (far - near);
		m_Projection[2][3] = 1.f;
		m_Projection[3][2] = -(far * near) / (far - near);
	}

	void Erupt::Camera::SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
	{
		// Orthonormal basis (unit length and intersecting at right angles)
		const glm::vec3 w{ glm::normalize(direction) };
		const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
		const glm::vec3 v{ glm::cross(w, u) };

		m_View = glm::mat4{ 1.f };
		m_View[0][0] = u.x;
		m_View[1][0] = u.y;
		m_View[2][0] = u.z;
		m_View[0][1] = v.x;
		m_View[1][1] = v.y;
		m_View[2][1] = v.z;
		m_View[0][2] = w.x;
		m_View[1][2] = w.y;
		m_View[2][2] = w.z;
		m_View[3][0] = -glm::dot(u, position);
		m_View[3][1] = -glm::dot(v, position);
		m_View[3][2] = -glm::dot(w, position);
	}

	void Erupt::Camera::SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up)
	{
		SetViewDirection(position, target - position, up);
	}

	void Erupt::Camera::SetViewYXZ(glm::vec3 position, glm::vec3 rotation)
	{
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
		const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
		const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
		m_View = glm::mat4{ 1.f };
		m_View[0][0] = u.x;
		m_View[1][0] = u.y;
		m_View[2][0] = u.z;
		m_View[0][1] = v.x;
		m_View[1][1] = v.y;
		m_View[2][1] = v.z;
		m_View[0][2] = w.x;
		m_View[1][2] = w.y;
		m_View[2][2] = w.z;
		m_View[3][0] = -glm::dot(u, position);
		m_View[3][1] = -glm::dot(v, position);
		m_View[3][2] = -glm::dot(w, position);
	}
}