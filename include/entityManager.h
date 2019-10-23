#pragma once
#include "entity.h"
#include "interval.h"
#include "archetype.h"
class EntityManager
{
	Entity* entities;
	IntervalSet intervals;
public:
	EntityManager(uint32_t size);
	EntityManager();


	void _free();
	Entity CreateEntity(Archetype componentMask, uint16_t poolId);
	void RemoveEntity(const Entity e);
	bool IsAlive(const Entity e) const;
	Entity* GetEntities()const;
};