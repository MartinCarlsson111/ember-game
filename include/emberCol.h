#pragma once
#include "glm/glm.hpp"
#include <xmmintrin.h>
class CollisionAlgos
{
public:
	inline static bool intersect(const glm::vec4& a, const glm::vec4& b)
	{

		return 
			(a.x + a.z > b.x &&
			 b.x + b.z > a.x) &&
			(a.y + a.w > b.y &&
			 b.y + b.w > a.y);
	}

	inline static bool intersect(const float& ax, const float& ay, const float& aw, const float& ah,
		const float& bx, const float& by, const float& bw, const float& bh)
	{
		return (ax + aw > bx&& bx + bw > ax) && (ay + ah > by&& by + bh > ay);
	}

	//TODO:https://www.gamedev.net/forums/topic/619296-ultra-fast-2d-aabb-overlap-test/?page=3


	inline static bool intersectSSE(const glm::vec4& a, const glm::vec4& b)
	{

		return false;
	}

};