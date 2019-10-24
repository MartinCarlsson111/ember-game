#include "movementSystem.h"
#include "componentList.h"
#include "input.h"
#include <iostream>

#include "tbb/tbb.h"

#include <xmmintrin.h>

const float speed2 = 20.0f;

struct applyVelocityJob {
	applyVelocityJob() {}
	void operator()(float dt, const Entity e, Position p, const Velocity v, ecs::ECS* ecs) {
		p.x += v.x * dt * speed2;
		p.y += v.y * dt * speed2;
		ecs->SetComponent<Position>(e, p);
	}
};

struct applyVelocityJobSIMD {
	applyVelocityJobSIMD() {}
	void operator()(const __m128 speedDelta, const Entity a, const Entity b, const __m128 p, const __m128 v, ecs::ECS* ecs) {
		__m128 dest = _mm_add_ps(p, _mm_mul_ps(v, speedDelta));
		float result[4];
		_mm_store_ps(result, dest);
		ecs->SetComponent<Position>(a, Position(result[0], result[1]));
		ecs->SetComponent<Position>(b, Position(result[2], result[3]));
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
	const float speed = 20.0f;

	__m128 speed3 = _mm_set_ps(dt * speed, dt * speed, dt * speed, dt * speed);
	applyVelocityJobSIMD applyVelSIMD = applyVelocityJobSIMD();
	tbb::parallel_for(
		tbb::blocked_range<size_t>(0, vel.comps.size() / 2),
		[&speed3, &speed, &applyVelSIMD, &pos, ecs, &vel](const tbb::blocked_range<size_t>& r) {
			for (size_t i = r.begin() * 2; i < r.end() * 2; i+=2)
			{
				__m128 p = _mm_set_ps(pos.comps[i + 1].y, pos.comps[i + 1].x, pos.comps[i].y, pos.comps[i].x);
				__m128 v = _mm_set_ps(vel.comps[i + 1].y, vel.comps[i + 1].x,  vel.comps[i].y, vel.comps[i].x);
				applyVelSIMD(speed3, pos.entities[i], pos.entities[i+1], p, v, ecs);
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
		playerVel.comps[i].x = Input::GetKey(KeyCode::A) ? -1.0f : Input::GetKey(KeyCode::D) ? 1.0f : 0.0f;
		playerVel.comps[i].y = Input::GetKey(KeyCode::W) ? 1.0f : Input::GetKey(KeyCode::S) ? -1.0f : 0.0f;
		ecs->SetComponent<Velocity>(playerVel.entities[i], playerVel.comps[i]);
	}
}
