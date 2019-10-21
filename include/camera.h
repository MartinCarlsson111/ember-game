#pragma once
#include "glm/glm.hpp"
#include "ecs.h"
#include "componentList.h"
class Camera
{
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec4 position;

#undef near
#undef far
	float fov, near, far, zoom;

	glm::vec3 up;
	glm::vec3 forward;
	glm::vec3 lookAt;

	glm::vec3 center;

public:

	Camera(glm::vec3 position, float fov, float near, float far);
	void Update(ecs::ECS* ecs);
	void SetPosition(glm::vec3 position);
	void Move(glm::vec3 direction);

	glm::mat4 GetProjection()const;
	glm::mat4 GetView() const;
	glm::mat4 GetVP() const;
	glm::vec4 GetPos() const;


	bool Contains(glm::vec2 pos, glm::vec2 size);

};