
#include "camera.h"
#include "glm/gtx/transform.hpp"
#include "input.h"
#include "engine.h"

#undef near
#undef far

const float zoomSpeed = 1.0f;

const float movementSpeed = 3.0f;

Camera::Camera(glm::vec3 position, float fov, float near, float far)
{
	this->zoom = 10.0f;
	this->near = near;
	this->fov = fov;
	this->far = far;
	this->position = glm::vec4(position, 1.0f);
	lookAt = this->position + glm::vec4(0, 0, 1, 0);
	view = glm::lookAt(position, lookAt, glm::vec3(0, 1, 0));
	projection = glm::ortho(0.0f, (float)Engine::wr.z, (float)Engine::wr.w, 0.0f, near, far);
}

#include <iostream>
void Camera::Update(float dt, ecs::ECS* ecs)
{
	//if (Input::GetKey(KeyCode::A))
	//{
	//	position.x -= movementSpeed * dt;
	//}
	//if (Input::GetKey(KeyCode::D))
	//{
	//	position.x += movementSpeed * dt;
	//}

	//if (Input::GetKey(KeyCode::W))
	//{
	//	position.y -= movementSpeed * dt;
	//}
	//if (Input::GetKey(KeyCode::S))
	//{
	//	position.y += movementSpeed * dt;
	//}

	auto arch = ecs->CreateArchetype<Position, Player>();

	auto playerPos = ecs->GetComponents<Position>(arch);
	float aspectRat = (float)Engine::wr.z/(float)Engine::wr.w;
	position = glm::vec4(playerPos.comps[0].x / (zoom * aspectRat), -playerPos.comps[0].y / zoom , 0, 0);
	//position = glm::vec4(playerPos.comps[0].x, -playerPos.comps[0].y, 0, 0) + glm::vec4(0.0f, 0, 0, 0);
	lookAt = position;

	view = glm::lookAt(lookAt, lookAt + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));

	projection = glm::ortho(-zoom*aspectRat, zoom*aspectRat, zoom, -zoom, near, far);
	//projection = glm::ortho(0.0f, aspectRat, 1.0f, 0.0f, near, far);
	//projection = glm::ortho(-zoom, zoom, zoom*aspectRat, -zoom*aspectRat, near, far);
	float mWheel = Input::GetMouseWheel();
	if (mWheel != 0)
	{
		zoom += mWheel* zoomSpeed;
		if (zoom <= 1)
		{
			zoom = 1.0f;
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
