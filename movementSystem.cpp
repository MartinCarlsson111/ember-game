#include "movementSystem.h"
#include "componentList.h"
#include "input.h"
#include <iostream>

#include "tbb/tbb.h"

#include <xmmintrin.h>

const float speed2 = 5.0f;

struct applyVelocityJob {
	void operator()(float dt, const Entity e, Position p, const Velocity v, ecs::ECS* ecs) {
		p.x += v.x * dt * speed2;
		p.y += v.y * dt * speed2;
		ecs->SetComponent<Position>(e, p);
	}
};

struct applyVelocityJobSIMD {
	void operator()(const __m128& speedDelta, Position& a, Position& b, const __m128& p, const __m128& v) {
		__m128 dest = _mm_add_ps(p, _mm_mul_ps(v, speedDelta));
		float result[4];
		_mm_store_ps(result, dest);
		a.x = result[0];
		a.y = result[1];
		b.x = result[2];
		b.y = result[3];
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
	//auto entities = ecs->GetEntityArray(archetype);
	//auto vel = ecs->GetComponents<Velocity>(archetype);

	//auto begin = std::chrono::high_resolution_clock::now();
	//auto pos = ecs->GetComponents<Position>(archetype);
		//auto begin = std::chrono::high_resolution_clock::now();
	//auto end = std::chrono::high_resolution_clock::now();
	//auto delta = end - begin;
	//auto ms = std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count() * 0.000001f;
	//std::cout << ms << std::endl;


	ecs::ECS::ComponentDataWrite<Position> posComps;
	ecs->GetComponentsWrite<Position>(archetype, posComps);


	ecs::ECS::ComponentDataWrite<Velocity> velComps;
	ecs->GetComponentsWrite<Velocity>(archetype, velComps);

	//for (int i = 0; i < vel.comps.size(); i++)
	//{
	//	for (int j = 0; j < vel.comps[i].size; j++)
	//	{
	//		pos.comps[i].data[j].x += vel.comps[i].data[j].x * speed * dt;
	//	}
	//}


	//auto begin = std::chrono::high_resolution_clock::now();
	const float speed = 20.0f;
	const float gravityConst = -1.0f;

	__m128 speed3 = _mm_set_ps(dt * speed, dt * speed, dt * speed, dt * speed);
	for (int i = 0; i < velComps.comps.size(); i++)
	{
		auto vel = velComps.comps[i];
		auto pos = posComps.comps[i];
		applyVelocityJobSIMD applyVelSIMD = applyVelocityJobSIMD();
		tbb::parallel_for(
			tbb::blocked_range<size_t>(0, vel.size < 8 ? 1 : vel.size / 8),
			[&speed3, &applyVelSIMD, &vel, &pos](const tbb::blocked_range<size_t>& r) {
				for (size_t i = r.begin() * 8; i < r.end() * 8; i += 8)
				{
					__m128 p = _mm_set_ps(pos.data[i+1].y, pos.data[i + 1].x, pos.data[i].y, pos.data[i].x);
					__m128 v = _mm_set_ps(vel.data[i+1].y, vel.data[i + 1].x, vel.data[i].y, vel.data[i].x);
					applyVelSIMD(speed3, pos.data[i], pos.data[i+1], p, v);

					__m128 p1 = _mm_set_ps(pos.data[i + 3].y, pos.data[i + 3].x, pos.data[i + 2].y, pos.data[i + 2].x);
					__m128 v1 = _mm_set_ps(vel.data[i + 3].y, vel.data[i + 3].x, vel.data[i + 2].y, vel.data[i + 2].x);
					applyVelSIMD(speed3, pos.data[i +2], pos.data[i + 3], p1, v1);

					__m128 p2 = _mm_set_ps(pos.data[i + 5].y, pos.data[i + 5].x, pos.data[i + 4].y, pos.data[i + 4].x);
					__m128 v2 = _mm_set_ps(vel.data[i + 5].y, vel.data[i + 5].x, vel.data[i + 4].y, vel.data[i + 4].x);
					applyVelSIMD(speed3, pos.data[i+4], pos.data[i + 5], p2, v2);

					__m128 p3 = _mm_set_ps(pos.data[i + 7].y, pos.data[i + 7].x, pos.data[i + 6].y, pos.data[i + 6].x);
					__m128 v3 = _mm_set_ps(vel.data[i + 7].y, vel.data[i + 7].x, vel.data[i + 6].y, vel.data[i + 6].x);
					applyVelSIMD(speed3, pos.data[i+6], pos.data[i + 7], p3, v3);
				}
			}
		);

	}/*
	auto end = std::chrono::high_resolution_clock::now();
	auto delta = end - begin;
	auto ms = std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count() * 0.000001f;
	std::cout << ms << std::endl;*/



	//__m128 speed3 = _mm_set_ps(dt * speed, dt * speed, dt * speed, dt * speed);
	//applyVelocityJobSIMD applyVelSIMD = applyVelocityJobSIMD();
	//tbb::parallel_for(
	//	tbb::blocked_range<size_t>(0, vel.comps.size() / 8),
	//	[&speed3, &speed, /*&entities*/ &applyVelSIMD, &pos, ecs, &vel](const tbb::blocked_range<size_t>& r) {
	//		for (size_t i = r.begin() * 8; i < r.end() * 8; i += 8)
	//		{
	//			__m128 p = _mm_set_ps(pos.comps[i + 1].y, pos.comps[i + 1].x, pos.comps[i].y, pos.comps[i].x);
	//			__m128 v = _mm_set_ps(vel.comps[i + 1].y, vel.comps[i + 1].x, vel.comps[i].y, vel.comps[i].x);
	//			applyVelSIMD(speed3, Entity(), Entity(), p, v, ecs);

	//			__m128 p1 = _mm_set_ps(pos.comps[i + 3].y, pos.comps[i + 3].x, pos.comps[i + 2].y, pos.comps[i + 2].x);
	//			__m128 v1 = _mm_set_ps(vel.comps[i + 3].y, vel.comps[i + 3].x, vel.comps[i + 2].y, vel.comps[i + 2].x);
	//			applyVelSIMD(speed3, Entity(), Entity(), p1, v1, ecs);

	//			__m128 p2 = _mm_set_ps(pos.comps[i + 5].y, pos.comps[i + 5].x, pos.comps[i + 4].y, pos.comps[i + 4].x);
	//			__m128 v2 = _mm_set_ps(vel.comps[i + 5].y, vel.comps[i + 5].x, vel.comps[i + 4].y, vel.comps[i + 4].x);
	//			applyVelSIMD(speed3, Entity(), Entity(), p2, v2, ecs);

	//			__m128 p3 = _mm_set_ps(pos.comps[i + 7].y, pos.comps[i + 7].x, pos.comps[i + 6].y, pos.comps[i + 6].x);
	//			__m128 v3 = _mm_set_ps(vel.comps[i + 7].y, vel.comps[i + 7].x, vel.comps[i + 6].y, vel.comps[i + 6].x);
	//			applyVelSIMD(speed3, Entity(), Entity(), p3, v3, ecs);
	//		}
	//	}
	//);


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
	auto playerEntity = ecs->GetEntityArray(playerArch);

	for (int i = 0; i < playerVel.comps.size(); i++)
	{
		playerVel.comps[i].x = Input::GetKey(KeyCode::A) ? -1.0f : Input::GetKey(KeyCode::D) ? 1.0f : 0.0f;
		playerVel.comps[i].y = Input::GetKey(KeyCode::W) ? 1.0f : Input::GetKey(KeyCode::S) ? -1.0f : 0.0f;
		ecs->SetComponent<Velocity>(playerEntity.entities[i], playerVel.comps[i]);
	}
}
