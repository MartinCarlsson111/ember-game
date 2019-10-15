#pragma once
#include "entity.h"
#include "interval.h"
class EntityManager
{
	Entity* entities;
	IntervalSet intervals;
public:
	EntityManager(uint32_t size);
	EntityManager();


	void _free();
	Entity CreateEntity();
	void RemoveEntity(const Entity e);
	bool IsAlive(const Entity e) const;
};