
#include "camera.h"
#include "glm/gtx/transform.hpp"
#include "input.h"
#include "engine.h"
#undef near
#undef far
Camera::Camera(glm::vec3 position, float fov, float near, float far)
{
	this->zoom = 9.5f;
	this->near = near;
	this->fov = fov;
	this->far = far;
	this->position = glm::vec4(position, 1.0f);
	lookAt = this->position + glm::vec4(0, 0, 1, 0);
	view = glm::lookAt(position, lookAt, glm::vec3(0, 1, 0));

	projection = glm::ortho(-zoom, zoom, -zoom, zoom, near, far);
}


void Camera::Update()
{
	if (Input::GetKey(KeyCode::A))
	{
		position.x += 0.1f;
	}
	if (Input::GetKey(KeyCode::D))
	{
		position.x -= 0.1f;
	}

	if (Input::GetKey(KeyCode::W))
	{
		position.y += 0.1f;
	}
	if (Input::GetKey(KeyCode::S))
	{
		position.y -= 0.1f;
	}

	lookAt = position + glm::vec4(0, 0, 1, 0);
	view = glm::lookAt(glm::vec3(position.x, position.y, position.z), lookAt, glm::vec3(0, 1, 0));
	projection = glm::ortho(-zoom, zoom, -zoom, zoom, near, far);

	float mWheel = Input::GetMouseWheel();
	if (mWheel != 0)
	{
		zoom += mWheel * 0.1f;
		if (zoom <= 10)
		{
			zoom = 10.0f;
		}
	}

}

void Camera::SetPosition(glm::vec3 position)
{
	this->position = glm::vec4(position, 1.0);
}

void Camera::Move(glm::vec3 direction)
{
	position += glm::vec4(direction, 1.0f);
}

glm::mat4 Camera::GetProjection() const
{
	return projection;
}

glm::mat4 Camera::GetView() const
{
	return view;
}

glm::mat4 Camera::GetVP() const
{
	return view * projection;
}

glm::vec4 Camera::GetPos() const
{
	return position;
}

bool Camera::Contains(glm::vec2 p, glm::vec2 size)
{
	glm::vec2 pos = glm::vec2(p.x + position.x, p.y + position.y);
	if (pos.x - size.x > -10 && pos.x + size.x < 10)
	{
		if (pos.y + size.y > 10 && pos.y - size.y < 10)
		{
			return true;
		}
	}
	return false;
}
