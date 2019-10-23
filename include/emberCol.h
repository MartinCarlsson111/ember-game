#pragma once
#include "glm/glm.hpp"
class CollisionAlgos
{
public:
	inline static bool intersect(glm::vec4 a, glm::vec4 b)
	{
		return (a.x + a.z > b.x &&
			b.x + b.z > a.x) &&
			(a.y + a.w > b.y &&
				b.y + b.w > a.y);
	}



};