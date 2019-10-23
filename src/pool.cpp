#include "pool.h"

void Pool::RemoveEntity(Entity e)
{
	used--;
	eManager->RemoveEntity(e);
}

Entity Pool::CreateEntity(Archetype type, uint16_t poolId)
{
	used++;
	return eManager->CreateEntity(type, poolId);;
}
