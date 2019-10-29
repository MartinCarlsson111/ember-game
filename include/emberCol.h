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

	//TODO:https://www.gamedev.net/forums/topic/619296-ultra-fast-2d-aabb-overlap-test/?page=3


	inline static __m128 intersectSSE(const glm::vec4& a, const glm::vec4& b)
	{
		const __m128 _minX = _mm_load1_ps(&a.x);
		const __m128 _minY = _mm_load1_ps(&a.y);
		const __m128 _maxX = _mm_load1_ps(&a.z);
		const __m128 _maxY = _mm_load1_ps(&a.w);

		const __m128 _bminX = _mm_load1_ps(&b.x);
		const __m128 _bminY = _mm_load1_ps(&b.y);
		const __m128 _bmaxX = _mm_load1_ps(&b.z);
		const __m128 _bmaxY = _mm_load1_ps(&b.w);

		return _mm_and_ps(
			_mm_and_ps(_mm_cmple_ps(_minX, _bmaxX), _mm_cmple_ps(_minY, _bmaxY)),
			_mm_and_ps(_mm_cmpge_ps(_maxX, _bminX), _mm_cmpge_ps(_maxY, _bminY)));

	}

};