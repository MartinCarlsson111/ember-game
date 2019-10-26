#include "broadphaseSystem.h"
#include "componentList.h"
#include <iostream>
#include "emberCol.h"
#include "tbb/tbb.h"

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

//struct BoxTest
//{
//	void operator()(int index, BroadPhaseSystem* system, const AABB& aAABB, Position& aPos, const ecs::ECS::ComponentDataWrite<Position>& pos, const ecs::ECS::ComponentDataWrite<AABB>& aabb)
//	{
//		for (int j = 0; j < pos.comps.size(); j++)
//		{
//			auto p2 = pos.comps[j];
//			auto ab2 = aabb.comps[j];
//
//			for (int k = index + 1; k < p2.size; k++)
//			{
//				auto b = p2.data[k];
//				auto baabb = ab2.data[k];
//				if (aAABB.isStatic && baabb.isStatic)
//				{
//					continue;
//				}
//				glm::vec4 _a = glm::vec4(aPos.x, aPos.y, aAABB.w, aAABB.h),
//					_b = glm::vec4(b.x, b.y, baabb.w, baabb.h);
//				if (CollisionAlgos::intersect(_a, _b))
//				{
//					if (!aAABB.isStatic)
//					{
//						BroadPhaseSystem::AABBMani A = { glm::vec2(aPos.x, aPos.y), glm::vec2(aPos.x - aAABB.w, aPos.y + aAABB.h) },
//							B = { glm::vec2(b.x, b.y), glm::vec2(b.x - baabb.w, b.y + baabb.h) };
//
//						glm::vec2 contactP = glm::vec2();
//						glm::vec2 normal = glm::vec2();
//						CalculateTOI(A, glm::vec2(0.05f, 0.05f), B, contactP, normal);
//						aPos.x -= normal.x * 0.5f;
//						aPos.y -= normal.y * 0.5f;
//
//						pos.comps[j].data[k].x -= -normal.x * 0.5f;
//						pos.comps[j].data[k].y -= -normal.y * 0.5f;
//					}
//				}
//			}
//		}
//	}
//};
struct BoxTest
{
	void operator()(std::vector<BroadPhaseSystem::SpatialObject>& spatialObjects)
	{
		for (int i = 0; i < spatialObjects.size(); i++)
		{
			auto p1 = spatialObjects[i];

			for (int j = i+1; j < spatialObjects.size(); j++)
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
					CalculateTOI(A, glm::vec2(0.05f, 0.05f), B, contactP, normal);
					if (!p1.aabb->isStatic)
					{
						p1.pos->x -= normal.x * 0.5f;
						p1.pos->y -= normal.y * 0.5f;
					}
					if (!p2.aabb->isStatic)
					{
						p2.pos->x += normal.x * 0.5f;
						p2.pos->y += normal.y * 0.5f;
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

	void operator()(std::vector<std::vector<BroadPhaseSystem::SpatialObject>>& buckets, Position& pos, AABB& aabb, const float& cellSize, const int& width)
	{
		int a = GetBucketId(pos.x, pos.y, cellSize, width);
		int b = GetBucketId(pos.x + aabb.w, pos.y, cellSize, width);
		int c = GetBucketId(pos.x, pos.y + aabb.h, cellSize, width);
		int d = GetBucketId(pos.x + aabb.w, pos.y + aabb.h, cellSize, width);
		BroadPhaseSystem::SpatialObject object = { &(pos), &(aabb) };
		if (a > 0 && a < buckets.size())
		{
			buckets[a].push_back(object);
		}
		if (b != a && b > 0 && b < buckets.size())
		{
			buckets[b].push_back(object);
		}
		if (c != b && c != a  && c > 0 && c < buckets.size())
		{
			buckets[c].push_back(object);
		}
		if (d != c && d != b && d != a && d > 0 && d < buckets.size())
		{
			buckets[d].push_back(object);
		}

		////buckets[a].push_back(BroadPhaseSystem::SpatialObject() = { &(*pos), &(*aabb)});
		////if (b != a)
		////{
		////	buckets[b].push_back(BroadPhaseSystem::SpatialObject() = { &(*pos), &(*aabb) });
		////}
		////if (c != b && c != a)
		////{
		////	buckets[c].push_back(BroadPhaseSystem::SpatialObject() = { &(*pos), &(*aabb) });
		////}
		////if (d != c && d != b && d != a)
		////{
		////	buckets[d].push_back(BroadPhaseSystem::SpatialObject() = { &(*pos), &(*aabb) });
		////}
	}
};



void BroadPhaseSystem::RunCollisionDetection(ecs::ECS* ecs)
{
	worldSize = 100000;
	bucketSize = 100;
	width = worldSize / bucketSize;

	using namespace tbb;
	using range = blocked_range<size_t>;
	//TODO: I want all dynamic entities to have a collision response component
	auto aabbEntities = ecs->CreateArchetype<Position, AABB, Dynamic>();
	ecs::ECS::ComponentDataWrite<Position> pos;
	ecs->GetComponentsWrite<Position>(aabbEntities, pos);

	ecs::ECS::ComponentDataWrite<AABB> aabb;
	ecs->GetComponentsWrite<AABB>(aabbEntities, aabb);

	buckets.resize(width * width);

#define PROFILEMS


#ifdef PROFILEMS
	auto begin = std::chrono::high_resolution_clock::now();
#endif
	std::fill(buckets.begin(), buckets.end(), std::vector<SpatialObject>());
#ifdef PROFILEMS
	//clear


	auto end = std::chrono::high_resolution_clock::now();
	auto delta = end - begin;
	auto ms = std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count() * 0.000001f;
	std::cout << "clear buckets: " << ms << std::endl;
#endif // PROFILEMS
	//create buckets
	//for (int i = 0; i < pos.comps.size(); i++)
	//{
	//	CalculateBucketSizes calculateBucketSizes;
	//	parallel_for(range(0, pos.comps[i].size),
	//		[&i, &calculateBucketSizes, this, &pos, &aabb](const range& r)
	//		{
	//			for (int j = r.begin(); j < r.end(); j++)
	//			{
	//				calculateBucketSizes(buckets, pos.comps[i].data[j], aabb.comps[i].data[j], (int)bucketSize, width);
	//			}
	//		}
	//	);
	//}

#ifdef PROFILEMS
	begin = std::chrono::high_resolution_clock::now();
#endif
	for (int i = 0; i < pos.comps.size(); i++)
	{
		CalculateBucketSizes calculateBucketSizes;
		for (int j = 0; j < pos.comps[i].size; j++)
		{
			calculateBucketSizes(buckets, pos.comps[i].data[j], aabb.comps[i].data[j], (int)bucketSize, width);
		}
	}
#ifdef PROFILEMS
	end = std::chrono::high_resolution_clock::now();
	delta = end - begin;
	ms = std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count() * 0.000001f;
	std::cout << "calculate bucket sizes: " << ms << std::endl;

	begin = std::chrono::high_resolution_clock::now();
#endif
	//query spatial hashing structure instead of pos.comps
		BoxTest boxTest;
		parallel_for(range(0, buckets.size()),
			[&boxTest, this](const range& r)
			{
				for (size_t j = r.begin(); j < r.end(); ++j)
				{
					boxTest(buckets[j]);
				}
			}
		);
#ifdef PROFILEMS
		end = std::chrono::high_resolution_clock::now();
		delta = end - begin;
		ms = std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count() * 0.000001f;

		std::cout << "box test: " << ms << std::endl;
#endif

}

void BroadPhaseSystem::UpdateStaticArray(ecs::ECS* ecs)
{
	auto staticEntities = ecs->CreateArchetype<Position, AABB, Static>();
	ecs::ECS::ComponentDataWrite<Position> staticPos;
	ecs::ECS::ComponentDataWrite<AABB> staticAABB;

	ecs->GetComponentsWrite<Position>(staticEntities, staticPos);
	ecs->GetComponentsWrite<AABB>(staticEntities, staticAABB);

	if (staticPos.totalSize != totalSizeStatic)
	{
		std::fill(staticBuckets.begin(), staticBuckets.end(), SpatialObject());

		//gather bucket sizes

		//create static buckets

		//assign buckets

		totalSizeStatic = staticPos.totalSize;
	}
}






void BroadPhaseSystem::Run(ecs::ECS* ecs)
{
	UpdateStaticArray(ecs);
	RunCollisionDetection(ecs);
}
