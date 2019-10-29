#pragma once
#include "ecs.h"
#include "componentList.h"
#include <atomic>
#include <vector>

template <typename T>
struct atomwrapper
{
	std::atomic<T> _a;

	atomwrapper()
		:_a()
	{}

	atomwrapper(const std::atomic<T>& a)
		:_a(a.load())
	{}

	atomwrapper(const atomwrapper& other)
		:_a(other._a.load())
	{}

	atomwrapper& operator=(const atomwrapper& other)
	{
		_a.store(other._a.load());
		return *this;
	}

	int operator++()
	{
		return _a.fetch_add(1);
	}

	void operator++(int)
	{
		_a.fetch_add(1);
	}
};


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
		int index;
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


	std::vector<SpatialObject> buckets;
	std::vector<atomwrapper<int>> bucketIndices;

	std::vector<SpatialObject> staticBuckets;
	std::vector<atomwrapper<int>> staticBucketSizes;

	int totalSizeStatic;

	int worldSize;
	int bucketSize;
	int width;

	//TODO: Deal with keys which are out of bounds
		//option a: Just let them exist in the last/first bucket
		//option b: dynamically resize world to fit
};