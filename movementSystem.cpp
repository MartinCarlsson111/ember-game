#include "movementSystem.h"
#include "componentList.h"
#include "input.h"
#include <iostream>

#include "tbb/tbb.h"


const float speed = 20.0f;

struct applyVelocityJob {
	applyVelocityJob() {}
	void operator()(float dt, const Entity e, Position p, const Velocity v, ecs::ECS* ecs) {
		p.x += v.x * dt * speed;
		p.y += v.y * dt * speed;
		ecs->SetComponent<Position>(e, p);
	}
};

//struct applyGravityJob
//{
//	applyGravityJob(){}
//	void operator()(const float gravityConst, const Entity e, Velocity v, ecs::ECS* ecs)
//	{
//		//ground check
//
//		v.y = gravityConst;
//		ecs->SetComponent<Velocity>(e, v);
//	}
//};

void MovementSystem::Move(float dt, ecs::ECS* ecs)
{
	auto archetype = ecs->CreateArchetype<Movable, Position, Velocity>();
	auto vel = ecs->GetComponents<Velocity>(archetype);
	auto pos = ecs->GetComponents<Position>(archetype);

	const float gravityConst = -1.0f;

	applyVelocityJob applyVel = applyVelocityJob();
	tbb::parallel_for(
		tbb::blocked_range<size_t>(0, vel.comps.size()),
		[&dt, &applyVel, &pos, ecs, &vel](const tbb::blocked_range<size_t>& r) {
			for (size_t i = r.begin(); i < r.end(); ++i)
			{
				applyVel(dt, pos.entities[i], pos.comps[i], vel.comps[i], ecs);
			}
		}
	);

	//applyGravityJob applyGravity = applyGravityJob();
	//tbb::parallel_for(
	//	tbb::blocked_range<size_t>(0, vel.comps.size()),
	//	[&applyGravity, &gravityConst, ecs, &vel](const tbb::blocked_range<size_t>& r) {
	//		for (size_t i = r.begin(); i < r.end(); ++i)
	//		{
	//			applyGravity(gravityConst, vel.entities[i], vel.comps[i], ecs);
	//		}
	//	}
	//);


	auto playerArch = ecs->CreateArchetype<Player, Position, Velocity>();
	auto playerVel = ecs->GetComponents<Velocity>(playerArch);
	auto playerPos = ecs->GetComponents<Position>(playerArch);

	for (int i = 0; i < playerVel.comps.size(); i++)
	{
		playerVel.comps[i].x = Input::GetKey(KeyCode::A) ? -1 : Input::GetKey(KeyCode::D) ? 1 : 0;
		playerVel.comps[i].y = Input::GetKey(KeyCode::W) ? 1 : Input::GetKey(KeyCode::S) ? -1 : 0;
		ecs->SetComponent<Velocity>(playerVel.entities[i], playerVel.comps[i]);
	}
}
