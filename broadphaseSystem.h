#pragma once
#include "ecs.h"
#include "componentList.h"
class BroadPhaseSystem
{

public:
	struct Manifold
	{
		uint16_t count;
		glm::vec2 depths;
		glm::vec2 contactPoint1;
		glm::vec2 n;
	};

	struct SpatialObject
	{
		Position* pos;
		AABB* aabb;
	};

	struct AABBMani
	{
		glm::vec2 min;
		glm::vec2 max;
	};
	void Run(ecs::ECS* ecs);

	void RunCollisionDetection(ecs::ECS* ecs);
	void UpdateStaticArray(ecs::ECS* ecs);


	std::vector<std::vector<SpatialObject>> buckets;

	std::vector<SpatialObject> staticBuckets;
	std::vector<int> staticBucketSizes;

	int totalSizeStatic;

	int worldSize;
	int bucketSize;
	int width;

	//TODO: Deal with keys which are out of bounds
		//option a: Just let them exist in the last/first bucket
		//option b: dynamically resize world to fit
};