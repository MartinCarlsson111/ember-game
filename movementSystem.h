#pragma once
#include "ecs.h"
class MovementSystem
{
public:

	void Move(float dt, ecs::ECS* ecs);
};