#include "pool.h"

void Pool::RemoveEntity(Entity e)
{
	used--;
	eManager.RemoveEntity(e);
}

Entity Pool::CreateEntity()
{
	used++;
	return eManager.CreateEntity();;
}
