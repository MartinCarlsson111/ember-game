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

	struct AABBMani
	{
		glm::vec2 min;
		glm::vec2 max;
	};

#define nullNode (-1)
	struct bPTreeNode
	{
		bool isLeaf()const {
			return child1 == nullNode;
		}

		void* userdata;
		AABB enlargedAABB;

		int32_t height;
		int32_t parent;
		int32_t child1;
		int32_t child2;
	};

	int32_t root;
	bPTreeNode* nodes;
public:
	void Run(ecs::ECS* ecs);
	void AABBtoAABBManifold(AABBMani A, AABBMani B, Manifold& m);
	float CalculateTOI(AABBMani a, glm::vec2 velocity, AABBMani b, glm::vec2& contactPoint, glm::vec2& n);

	//gather entities

	//run broadphase
};