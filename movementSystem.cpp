#include "movementSystem.h"
#include "componentList.h"
#include "input.h"
#include <iostream>
void MovementSystem::Move(ecs::ECS* ecs)
{
	auto archetype = ecs->CreateArchetype<Movable, Position, Velocity>();
	auto vel = ecs->GetComponents<Velocity>(archetype);
	auto pos = ecs->GetComponents<Position>(archetype);

	for (int i = 0; i < pos.comps.size(); i++)
	{
		pos.comps[i].x += vel.comps[i].x;
		pos.comps[i].y += vel.comps[i].y;

		ecs->SetComponent<Position>(pos.entities[i], Position(pos.comps[i].x, pos.comps[i].y));
	}

	auto playerArch = ecs->CreateArchetype<Player, Position, Velocity>();
	auto playerVel = ecs->GetComponents<Velocity>(playerArch);

	for (int i = 0; i < playerVel.comps.size(); i++)
	{
		playerVel.comps[i].x = Input::GetKey(KeyCode::A) ? 1 : Input::GetKey(KeyCode::D) ? -1 : 0;
		ecs->SetComponent<Velocity>(playerVel.entities[i], playerVel.comps[i]);
	}	
}
