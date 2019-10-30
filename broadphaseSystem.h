#pragma once
#include "ecs.h"
#include "componentList.h"
#include <atomic>
#include <vector>
#include "profiler.h"

#define ConcurrentHashMap

#include "tbb/concurrent_hash_map.h"
#include "tbb/concurrent_vector.h"

struct SpatialObject
{
	int index;
	Position* pos;
	AABB* aabb;
	Velocity* vel;
};

using BucketStructure = tbb::concurrent_vector<tbb::concurrent_vector<SpatialObject>>;

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



	struct AABBMani
	{
		glm::vec2 min;
		glm::vec2 max;
	};
	void Run(ecs::ECS* ecs);

	void RunCollisionDetection(ecs::ECS* ecs);
	void UpdateStaticArray(ecs::ECS* ecs);


#ifndef ConcurrentHashMap

	std::vector<SpatialObject> buckets;
	std::vector<atomwrapper<int>> bucketIndices;

	std::vector<SpatialObject> staticBuckets;
	std::vector<atomwrapper<int>> staticBucketSizes;

	int totalSizeStatic;

#else

	BucketStructure buckets;
	BucketStructure staticBuckets;
#endif // !ConcurrentHashMap
	int worldSize;
	int bucketSize;
	int width;
	int nBuckets;

	Profiler profiler;

	//TODO: Deal with keys which are out of bounds
		//option a: Just let them exist in the last/first bucket
		//option b: dynamically resize world to fit
};