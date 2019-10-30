#include "broadphaseSystem.h"
#include "componentList.h"
#include <iostream>
#include "emberCol.h"
#include "tbb/tbb.h"
#define SpatialHashing
#define PROFILEMS
static void AABBtoAABBManifold(BroadPhaseSystem::AABBMani A, BroadPhaseSystem::AABBMani B, BroadPhaseSystem::Manifold& m)
{
	m = BroadPhaseSystem::Manifold() =
	{
		0, glm::vec2(0)
	};
	glm::vec2 mid_a = (A.min + A.max) * 0.5f;
	glm::vec2 mid_b = (B.min + B.max) * 0.5f;
	glm::vec2 eA = glm::abs((A.max - A.min) * 0.5f);
	glm::vec2 eB = glm::abs((B.max - B.min) * 0.5f);
	glm::vec2 d = mid_b - mid_a;

	// calc overlap on x and y axes
	float dx = eA.x + eB.x - glm::abs(d.x);
	if (dx < 0) return;
	float dy = eA.y + eB.y - glm::abs(d.y);
	if (dy < 0) return;

	glm::vec2 n = glm::vec2();
	float depth = 0;
	glm::vec2 p = glm::vec2(0); //+ glm::vec2(dx, dy);

	// x axis overlap is smaller
	if (dx < dy)
	{
		depth = dx;
		if (d.x < 0)
		{
			n = glm::vec2(-1.0f, 0);
			p.x = mid_a.x - eA.x;
		}
		else
		{
			n = glm::vec2(1.0f, 0);
			p.x = mid_a.x + eA.x;
		}
	}

	// y axis overlap is smaller
	else
	{
		depth = dy;
		if (d.y < 0)
		{
			n = glm::vec2(0, -1.0f);
			p.y = mid_a.y - eA.y;
		}
		else
		{
			n = glm::vec2(0, 1.0f);
			p.y = mid_a.y + eA.y;
		}
	}

	m.count = 1;
	m.contactPoint1 = p;
	m.depths[0] = depth;
	m.n = n;
}

static float CalculateTOI(BroadPhaseSystem::AABBMani a, glm::vec2 velocity, BroadPhaseSystem::AABBMani b, glm::vec2& contactPoint, glm::vec2& n)
{
	//test if A and B collide within maxTime

	glm::vec2 maxVelocity = glm::vec2();
	glm::vec2 d = velocity / maxVelocity;
	int nSteps = d.x > d.y ? (int)d.x : (int)d.y;
	glm::vec2 aContactPoint = glm::vec2();
	//glm::vec2 bContactPoint = glm::vec2();
	glm::vec2 normal = glm::vec2();
	//test if velocity is within maxRange

	float t = 0;
	int i = 0;
	do
	{
		a.min += velocity;
		a.max += velocity;
		BroadPhaseSystem::Manifold m = BroadPhaseSystem::Manifold();
		AABBtoAABBManifold(a, b, m);
		if (m.count != 0)
		{
			aContactPoint = m.contactPoint1;
			normal = m.n;
			t = 1;
			break;
		}
		i++;
	} while (i < nSteps);

	contactPoint = aContactPoint;
	n = normal;

	return t >= 1 ? 1 : t;
}


#ifndef ConcurrentHashMap

#pragma region Jobs
struct BoxTestReference
{
	void operator()(int index, BroadPhaseSystem* system, const AABB& aAABB, Position& aPos, const ecs::ECS::ComponentDataWrite<Position>& pos, const ecs::ECS::ComponentDataWrite<AABB>& aabb)
	{
		for (int j = 0; j < pos.comps.size(); j++)
		{
			auto p2 = pos.comps[j];
			auto ab2 = aabb.comps[j];

			for (int k = index + 1; k < p2.size; k++)
			{
				auto b = p2.data[k];
				auto baabb = ab2.data[k];
				if (aAABB.isStatic && baabb.isStatic)
				{
					continue;
				}
				glm::vec4 _a = glm::vec4(aPos.x, aPos.y, aAABB.w, aAABB.h),
					_b = glm::vec4(b.x, b.y, baabb.w, baabb.h);
				if (CollisionAlgos::intersect(_a, _b))
				{
					if (!aAABB.isStatic)
					{
						BroadPhaseSystem::AABBMani A = { glm::vec2(aPos.x, aPos.y), glm::vec2(aPos.x - aAABB.w, aPos.y + aAABB.h) },
							B = { glm::vec2(b.x, b.y), glm::vec2(b.x - baabb.w, b.y + baabb.h) };

						glm::vec2 contactP = glm::vec2();
						glm::vec2 normal = glm::vec2();
						CalculateTOI(A, glm::vec2(0.05f, 0.05f), B, contactP, normal);
						aPos.x -= normal.x * 0.5f;
						aPos.y -= normal.y * 0.5f;

						pos.comps[j].data[k].x -= -normal.x * 0.5f;
						pos.comps[j].data[k].y -= -normal.y * 0.5f;
					}
				}
			}
		}
	}
};

struct BoxTest
{
	void operator()(const std::vector<SpatialObject>& spatialObjects, const SpatialObject& p1, const int& i, const int& end)
	{
		for (int j = i + 1; j < end; j++)
		{
			auto p2 = spatialObjects[j];

			if (p1.aabb->isStatic && p2.aabb->isStatic)
			{
				continue;
			}
			glm::vec4 _a = glm::vec4(p1.pos->x, p1.pos->y, p1.aabb->w, p1.aabb->h),
				_b = glm::vec4(p2.pos->x, p2.pos->y, p2.aabb->w, p2.aabb->h);
			if (CollisionAlgos::intersect(_a, _b))
			{
				BroadPhaseSystem::AABBMani A = { glm::vec2(p1.pos->x, p1.pos->y), glm::vec2(p1.pos->x - p1.aabb->w, p1.pos->y + p1.aabb->h) },
					B = { glm::vec2(p2.pos->x, p2.pos->y), glm::vec2(p2.pos->x - p2.aabb->w, p2.pos->y + p2.aabb->h) };
				glm::vec2 contactP = glm::vec2();
				glm::vec2 normal = glm::vec2();
				const float tolerance = 0.5f;

				CalculateTOI(A, glm::vec2(0.05f, 0.05f), B, contactP, normal);
				if (!p1.aabb->isStatic)
				{
					p1.pos->x -= normal.x * tolerance;
					p1.pos->y -= normal.y * tolerance;
				}
				if (!p2.aabb->isStatic)
				{
					p2.pos->x += normal.x * tolerance;
					p2.pos->y += normal.y * tolerance;
				}
			}
		}
	}
};


struct StaticBoxTest
{
	void operator()(const std::vector<SpatialObject>& spatialObjects, const std::vector<atomwrapper<int>>& indices, const AABB& a1, Position* p1)
	{
		for (int i = 0; i < 4; i++)
		{
			if ((a1.indices[i] - 1) > 0 && (a1.indices[i] - 1) < indices.size())
			{
				int end = indices[a1.indices[i]]._a.load();
				for (int a = indices[a1.indices[i] - 1]._a.load(); a < end; a++)
				{
					auto b = spatialObjects[a];
					glm::vec4 _a = glm::vec4(p1->x, p1->y, a1.w, a1.h),
						_b = glm::vec4(b.pos->x, b.pos->y, b.aabb->w, b.aabb->h);

					if (CollisionAlgos::intersect(_a, _b))
					{
						BroadPhaseSystem::AABBMani A = { glm::vec2(p1->x, p1->y), glm::vec2(p1->x - a1.w, p1->y + a1.h) },
							B = { glm::vec2(b.pos->x, b.pos->y), glm::vec2(b.pos->x - b.aabb->w, b.pos->y + b.aabb->h) };
						glm::vec2 contactP = glm::vec2();
						glm::vec2 normal = glm::vec2();
						const float tolerance = 0.5f;

						CalculateTOI(A, glm::vec2(0.05f, 0.05f), B, contactP, normal);
						if (!a1.isStatic)
						{
							p1->x -= normal.x * tolerance;
							p1->y -= normal.y * tolerance;
						}
					}
				}
			}
		}
	}
};

struct CalculateBucketSizes
{
	int GetBucketId(const float& px, const float& py, const float& cellSize, const int& width) const
	{
		return (int)(std::floor(px / cellSize) + std::floor(py / cellSize) * width);
	}

	void operator()(std::vector<atomwrapper<int>>* buckets, const Position& pos, AABB* aabb, const float& cellSize, const int& width)
	{
		aabb->indices[0] = GetBucketId(pos.x, pos.y, cellSize, width);
		aabb->indices[1] = GetBucketId(pos.x + aabb->w, pos.y, cellSize, width);
		aabb->indices[2] = GetBucketId(pos.x, pos.y + aabb->h, cellSize, width);
		aabb->indices[3] = GetBucketId(pos.x + aabb->w, pos.y + aabb->h, cellSize, width);

		auto a = aabb->indices[0];
		auto b = aabb->indices[1];
		auto c = aabb->indices[2];
		auto d = aabb->indices[3];

		auto bucketSize = buckets->size();

		if (a >= 0 && a < bucketSize)
		{
			buckets->operator[](a)++;
		}
		if (b != a && b >= 0 && b < bucketSize)
		{
			buckets->operator[](b)++;
		}
		if (c != b && c != a && c >= 0 && c < bucketSize)
		{
			buckets->operator[](c)++;
		}
		if (d != c && d != b && d != a && d >= 0 && d < bucketSize)
		{
			buckets->operator[](d)++;
		}
	}
};


struct FillBuckets
{

	void operator()(std::vector<atomwrapper<int>>* bucketIndices, AABB* aabb, Position* pos, std::vector<SpatialObject>* buckets)
	{
		uint64_t bucketSize = bucketIndices->size();
		auto a = aabb->indices[0];
		auto b = aabb->indices[1];
		auto c = aabb->indices[2];
		auto d = aabb->indices[3];

		if (a >= 0 && a < bucketSize)
		{
			int index = ++bucketIndices->operator[](a);
			buckets->operator[](index).pos = pos;
			buckets->operator[](index).aabb = aabb;
			buckets->operator[](index).index = a;
		}
		if (b != a && b >= 0 && b < bucketSize)
		{
			int index = ++bucketIndices->operator[](b);
			buckets->operator[](index).pos = pos;
			buckets->operator[](index).aabb = aabb;
			buckets->operator[](index).index = b;

		}
		if (c != b && c != a && c >= 0 && c < bucketSize)
		{
			int index = ++bucketIndices->operator[](c);
			buckets->operator[](index).pos = pos;
			buckets->operator[](index).aabb = aabb;
			buckets->operator[](index).index = c;
		}
		if (d != c && d != b && d != a && d >= 0 && d < bucketSize)
		{
			int index = ++bucketIndices->operator[](d);
			buckets->operator[](index).pos = pos;
			buckets->operator[](index).aabb = aabb;
			buckets->operator[](index).index = d;
		}
	}

	void operator()(std::vector<atomwrapper<int>>* bucketIndices, AABB* aabb, Position* pos, Velocity* vel, std::vector<SpatialObject>* buckets)
	{
		uint64_t bucketSize = bucketIndices->size();
		auto a = aabb->indices[0];
		auto b = aabb->indices[1];
		auto c = aabb->indices[2];
		auto d = aabb->indices[3];

		if (a >= 0 && a < bucketSize)
		{
			int index = ++bucketIndices->operator[](a);
			buckets->operator[](index).pos = pos;
			buckets->operator[](index).aabb = aabb;
			buckets->operator[](index).vel = vel;
			buckets->operator[](index).index = a;
		}
		if (b != a && b >= 0 && b < bucketSize)
		{
			int index = ++bucketIndices->operator[](b);
			buckets->operator[](index).pos = pos;
			buckets->operator[](index).aabb = aabb;
			buckets->operator[](index).vel = vel;
			buckets->operator[](index).index = b;

		}
		if (c != b && c != a && c >= 0 && c < bucketSize)
		{
			int index = ++bucketIndices->operator[](c);
			buckets->operator[](index).pos = pos;
			buckets->operator[](index).aabb = aabb;
			buckets->operator[](index).vel = vel;
			buckets->operator[](index).index = c;
		}
		if (d != c && d != b && d != a && d >= 0 && d < bucketSize)
		{
			int index = ++bucketIndices->operator[](d);
			buckets->operator[](index).pos = pos;
			buckets->operator[](index).aabb = aabb;
			buckets->operator[](index).vel = vel;
			buckets->operator[](index).index = d;
		}
	}
};


#pragma endregion



#else
#pragma region Jobs
struct Boxtest
{
	void operator()()
	{

	}
};

struct FillBuckets
{
	int GetBucketId(const float& px, const float& py, const float& cellSize, const int& width) const
	{
		return (int)(std::floor(px / cellSize) + std::floor(py / cellSize) * width);
	}

	void operator()(BucketStructure* buckets, Position* p, AABB* a, const int& cellSize, const int& width)
	{
		const static int indexCount = 4;
		int indices[indexCount] = { 0, 0,0 ,0 };
		indices[0] = GetBucketId(p->x, p->y, cellSize, width);
		indices[1] = GetBucketId(p->x + a->w, p->y, cellSize, width);
		indices[2] = GetBucketId(p->x, p->y + a->h, cellSize, width);
		indices[3] = GetBucketId(p->x + a->w, p->y + a->h, cellSize, width);
		bool duplicate = false;
		for (int i = 0; i < indexCount; i++)
		{
			if (indices[i] > buckets->size() || indices[i] < 0)
			{
				continue;
			}
			for (int j = 0; j < i; j++)
			{
				if (indices[i] == indices[j])
				{
					duplicate = true;
					break;
				}
			}
			if (!duplicate)
			{
				buckets->operator[](indices[i]).push_back(SpatialObject() = { 0, p, a, nullptr });
			}
			duplicate = false;
		}
	}
};

#pragma endregion
#endif // !ConcurrentHashMap






void BroadPhaseSystem::RunCollisionDetection(ecs::ECS* ecs)
{


	using namespace tbb;
	using range = blocked_range<size_t>;

	//TODO: I want all dynamic entities to have a collision response component
	auto aabbEntities = ecs->CreateArchetype<Position, AABB, Dynamic, Velocity>();
	ecs::ECS::ComponentDataWrite<Position> pos;
	ecs->GetComponentsWrite<Position>(aabbEntities, pos);

	ecs::ECS::ComponentDataWrite<Velocity> vel;
	ecs->GetComponentsWrite<Velocity>(aabbEntities, vel);

	ecs::ECS::ComponentDataWrite<AABB> aabb;
	ecs->GetComponentsWrite<AABB>(aabbEntities, aabb);

#ifndef ConcurrentHashMap
#ifndef SpatialHashing
	BoxTestReference boxTestReference;

	for (int i = 0; i < pos.comps.size(); i++)
	{
		tbb::parallel_for(range(0, pos.comps[i].size),
			[&i, &boxTestReference, this, &pos, &aabb](const range& r)
			{
				for (int j = r.begin(); j < r.end(); j++)
				{
					boxTestReference(j, this, aabb.comps[i].data[j], pos.comps[i].data[j], pos, aabb);
				}
			}
		);
	}


#endif // !SpatialHashing

#ifdef SpatialHashing


	const int vertices = 4;

#ifdef PROFILEMS
	profiler.Start("Reset Buckets");

#endif
	bucketIndices.resize((uint64_t)width * width);
	std::fill(bucketIndices.begin(), bucketIndices.end(), std::atomic_int(0));
	if (buckets.size() != (uint64_t)pos.totalSize * vertices)
	{
		buckets.resize((uint64_t)pos.totalSize * vertices);
	}
	std::fill(buckets.begin(), buckets.end(), SpatialObject());
#ifdef PROFILEMS
	//clear	
	profiler.End();
	profiler.Start("Calculate Bucket Sizes");
#endif

	for (int i = 0; i < pos.comps.size(); i++)
	{
		CalculateBucketSizes calculateBucketSizes;
		parallel_for(range(0, pos.comps[i].size),
			[&i, &calculateBucketSizes, this, &pos, &aabb](const range& r)
			{
				for (int j = r.begin(); j < r.end(); j++)
				{
					calculateBucketSizes(&bucketIndices, pos.comps[i].data[j], aabb.comps[i].data + j, bucketSize, width);
				}
			}
		);
	}

#ifdef PROFILEMS
	profiler.End();
	profiler.Start("single thread fix indices");

#endif // PROFILEMS
	int lastEnd = 0;
	int lastStart = 0;
	for (int i = 0; i < bucketIndices.size(); i++)
	{
		lastStart = bucketIndices[i]._a.load();
		bucketIndices[i]._a = lastEnd;
		lastEnd = bucketIndices[i]._a + lastStart;
	}
#ifdef PROFILEMS
	profiler.End();
	profiler.Start("Fill Buckets");
#endif // PROFILEMS
	for (int i = 0; i < pos.comps.size(); i++)
	{
		FillBuckets fillBuckets;
		parallel_for(range(0, pos.comps[i].size),
			[&i, &fillBuckets, this, &pos, &aabb, &vel](const range& r)
			{
				for (int j = r.begin(); j < r.end(); j++)
				{
					fillBuckets(&bucketIndices, (aabb.comps[i].data + j), pos.comps[i].data + j, vel.comps[i].data + j, &buckets);
				}
			}
		);
	}
#ifdef PROFILEMS
	profiler.End();
	profiler.Start("Box test");
#endif
	//query spatial hashing structure instead of pos.comps
	BoxTest boxTest;
	parallel_for(range(0, bucketIndices.size()),
		[&boxTest, this](const range& r)
		{
			for (int i = r.begin(); i < r.end(); i++)
			{
				auto end = ((uint64_t)i + 1) >= bucketIndices.size() ? 0 : bucketIndices[(uint64_t)i + 1]._a.load();
				parallel_for(range(bucketIndices[i]._a.load(), end),
					[&boxTest, this, &i, &end](const range& r) {
						for (int j = r.begin(); j < r.end(); j++)
						{
							auto p1 = buckets[j];
							boxTest(buckets, p1, j, end);
						}

					});
			}
		}
	);

	StaticBoxTest staticBoxTest;
	parallel_for(range(0, pos.comps.size()),
		[&staticBoxTest, this, &pos, &aabb](const range& r)
		{
			for (int i = r.begin(); i < r.end(); i++)
			{
				for (int j = 0; j < pos.comps[i].size; j++)
				{
					staticBoxTest(staticBuckets, staticBucketSizes, aabb.comps[i].data[j], &pos.comps[i].data[j]);
				}
			}
		}
	);


#ifdef PROFILEMS
	profiler.End();
#endif
#endif // SpatialHashing
#else
	if (buckets.size() != nBuckets)
	{
		buckets.clear();
		for (int i = 0; i < nBuckets; i++)
		{
			buckets.emplace_back(tbb::concurrent_vector<SpatialObject>());
		}
	}
	profiler.Start("Clear buckets");
	tbb::parallel_for(range(0, buckets.size()), [this](const range& r) {

		for (int i = r.begin(); i < r.end(); i++)
		{
			buckets[i].clear();
		}
		});
	profiler.End();



	profiler.Start("Fill Buckets");
	FillBuckets fillBuckets;
	for (int i = 0; i < pos.comps.size(); i++)
	{
		tbb::parallel_for(range(0, pos.comps[i].size), [&i, this, &pos, &aabb, &fillBuckets](const range& r) {

			for (int j = r.begin(); j < r.end(); j++)
			{
				fillBuckets(&buckets, pos.comps[i].data + j, aabb.comps[i].data + j, bucketSize, width);
			}
			});
	}
	profiler.End();

	//uint64_t counter = 0;
	//for (int i = 0; i < buckets.size(); i++)
	//{
	//	counter += buckets[i].size();
	//}
	//std::cout << counter << std::endl;
	

#endif // !ConcurrentHashMap
}

void BroadPhaseSystem::UpdateStaticArray(ecs::ECS* ecs)
{
	using namespace tbb;
	using range = blocked_range<size_t>;
	auto staticEntities = ecs->CreateArchetype<Position, AABB, Static>();
	ecs::ECS::ComponentDataWrite<Position> staticPos;
	ecs::ECS::ComponentDataWrite<AABB> staticAABB;

	ecs->GetComponentsWrite<Position>(staticEntities, staticPos);
	ecs->GetComponentsWrite<AABB>(staticEntities, staticAABB);




#ifndef ConcurrentHashMap
	if (staticPos.totalSize != totalSizeStatic)
	{
		const int vertices = 4;
		staticBuckets.resize((uint64_t)staticPos.totalSize * vertices);
		std::fill(staticBuckets.begin(), staticBuckets.end(), SpatialObject());
		staticBucketSizes.resize((uint64_t)width * width);
		std::fill(staticBucketSizes.begin(), staticBucketSizes.end(), std::atomic_int(0));
		
		for (int i = 0; i < staticPos.comps.size(); i++)
		{
			CalculateBucketSizes calculateBucketSizes;
			parallel_for(range(0, staticPos.comps[i].size),
				[&i, &calculateBucketSizes, this, &staticPos, &staticAABB](const range& r)
				{
					for (int j = r.begin(); j < r.end(); j++)
					{
						calculateBucketSizes(&staticBucketSizes, staticPos.comps[i].data[j], staticAABB.comps[i].data + j, bucketSize, width);
					}
				}
			);
		}

		int lastEnd = 0;
		int lastStart = 0;
		for (int i = 0; i < staticBucketSizes.size(); i++)
		{
			lastStart = staticBucketSizes[i]._a.load();
			staticBucketSizes[i]._a = lastEnd;
			lastEnd = staticBucketSizes[i]._a + lastStart;
		}

		uint64_t staticPosCompsSize = staticPos.comps.size();

		for (int i = 0; i < staticPosCompsSize; i++)
		{
			FillBuckets fillBuckets;
			parallel_for(range(0, staticPos.comps[i].size),
				[&i, &fillBuckets, this, &staticPos, &staticAABB](const range& r)
				{
					for (int j = r.begin(); j < r.end(); j++)
					{
						fillBuckets(&staticBucketSizes, (staticAABB.comps[i].data + j), staticPos.comps[i].data + j, &staticBuckets);
					}
				}
			);
		}

		totalSizeStatic = staticPos.totalSize;
	}

#else

#endif // !ConcurrentHashMap
}

void BroadPhaseSystem::Run(ecs::ECS* ecs)
{
	worldSize = 20000;
	bucketSize = 20;
	width = worldSize / bucketSize;
	nBuckets = width * width;


	UpdateStaticArray(ecs);
	RunCollisionDetection(ecs);
}
