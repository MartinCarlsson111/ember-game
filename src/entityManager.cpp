#include "entityManager.h"

EntityManager::EntityManager(uint32_t size)
{
	entities = (Entity*)calloc(size, sizeof(Entity));
	intervals = IntervalSet(0, size);
}

EntityManager::EntityManager(): entities(nullptr)
{

}



void EntityManager::_free()
{
	if (entities != nullptr)
	{
		free(entities);
		entities = nullptr;
	}
}

Entity EntityManager::CreateEntity(Archetype type, uint16_t poolId)
{
	uint32_t index = intervals.GetFirst();
	Entity e = Entity(index, entities[index].version(), type.types(), poolId);
	entities[index] = e;
	return e;
}

void EntityManager::RemoveEntity(const Entity e)
{
	intervals.Add(e.index());
	entities[e.index()].incrementVersion();
}

bool EntityManager::IsAlive(const Entity e) const
{
	if (e.version() == entities[e.index()].version())
	{
		return true;
	}
	return false;
}

Entity* EntityManager::GetEntities() const
{
	return entities;
}
