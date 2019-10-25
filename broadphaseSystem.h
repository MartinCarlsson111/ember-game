#pragma once
#include "ecs.h"
#include "componentList.h"
class BroadPhaseSystem
{
	struct Manifold
	{
		uint16_t count;
		glm::vec2 depths;
		glm::vec2 contactPoint1;
		glm::vec2 n;
	};



public:
	struct AABBMani
	{
		glm::vec2 min;
		glm::vec2 max;
	};
	void Run(ecs::ECS* ecs);
	void AABBtoAABBManifold(AABBMani A, AABBMani B, Manifold& m);
	float CalculateTOI(AABBMani a, glm::vec2 velocity, AABBMani b, glm::vec2& contactPoint, glm::vec2& n);

	//gather entities

	//run broadphase
};