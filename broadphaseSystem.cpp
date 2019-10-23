#include "broadphaseSystem.h"
#include "componentList.h"
#include <iostream>
#include "emberCol.h"



void BroadPhaseSystem::AABBtoAABBManifold(AABBMani A, AABBMani B, Manifold& m)
{
	m = Manifold() =
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

float BroadPhaseSystem::CalculateTOI(AABBMani a, glm::vec2 velocity, AABBMani b, glm::vec2& contactPoint, glm::vec2& n)
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
		Manifold m = Manifold();
		AABBtoAABBManifold(a, b, m);
		if (m.count != 0)
		{
			aContactPoint = m.contactPoint1;
			//depth? 
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

void BroadPhaseSystem::Run(ecs::ECS* ecs)
{
	auto aabbEntities = ecs->CreateArchetype<Position, AABB>();

	auto aabbs = ecs->GetComponents<AABB>(aabbEntities);
	auto pos = ecs->GetComponents<Position>(aabbEntities);


	unsigned long long count = 0;
	for (int i = 0; i < aabbs.comps.size(); i++)
	{
		for (int x = i + 1; x < aabbs.comps.size(); x++)
		{
			glm::vec4 a = glm::vec4(pos.comps[i].x, pos.comps[i].y, aabbs.comps[i].w, aabbs.comps[i].h),
				b = glm::vec4(pos.comps[x].x, pos.comps[x].y, aabbs.comps[x].w, aabbs.comps[x].h);
			if (CollisionAlgos::intersect(a, b))
			{
				if (!aabbs.comps[i].isStatic)
				{
					AABBMani A = { glm::vec2(pos.comps[i].x, pos.comps[i].y), glm::vec2(pos.comps[i].x - aabbs.comps[i].w, pos.comps[i].y + aabbs.comps[i].h) },
						B = { glm::vec2(pos.comps[x].x, pos.comps[x].y), glm::vec2(pos.comps[x].x - aabbs.comps[x].w, pos.comps[x].y + aabbs.comps[x].h) };

					glm::vec2 contactP = glm::vec2();
					glm::vec2 normal = glm::vec2();
					CalculateTOI(A, glm::vec2(0.05f, 0.05f), B, contactP, normal);

					pos.comps[i].x -= normal.x * 0.05f;
					pos.comps[i].y -= normal.y * 0.05f;

					ecs->SetComponent<Position>(pos.entities[i], pos.comps[i]);
				}
			}
		}
	}




}
