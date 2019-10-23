#include "..\include\ecs.h"

ecs::ECS::ECS()
{
}

ecs::ECS::~ECS()
{
	for (int i = 0; i < MAXCOMPONENTS; i++)
	{
		if (components[i].data != NULL)
		{
			components[i]._free();
		}
	}
	for (int i = 0; i < pools.size(); i++)
	{
		pools[i]._free();
	}
}

uint32_t ecs::ECS::findLowestSizeHint(Archetype archetype)
{
	uint32_t lowestHint = UINT32_MAX;
	uint64_t types = archetype.types();

	for (int i = 0; i < MAXCOMPONENTS; i++)
	{
		if ((types >> i) & 1U)
		{
			if (hints[i] != 0)
			{
				if (hints[i] < lowestHint)
				{
					lowestHint = hints[i];
				}
			}
		}
	}
	return lowestHint;
}


//TODO: This should be turned upside down
//instead of allocating by pool size, allocate massive buffer, then split it up with the pools

void ecs::ECS::CreatePool(Archetype type)
{
	uint32_t componentArrayIndices[MAXCOMPONENTS] = { 0 };
	uint32_t lowestHint = findLowestSizeHint(type);
	uint32_t allocSize = lowestHint > poolSize ? poolSize : lowestHint;

	for (uint32_t i = 0; i < MAXCOMPONENTS; i++)
	{
		uint64_t types = type.types();
		if ((types >> i) & 1U)
		{
			auto index = components[i].used;

			if (index + allocSize >= components[i].allocations)
			{
				components[i].allocre(allocSize);
			}

			componentArrayIndices[i] = index;
			components[i].used += allocSize;
		}
	}
	Pool pool; 
	pools.push_back(*&pool);
	pools.back().Init(type, allocSize, componentArrayIndices);
}





void ecs::ECS::CreateEntitiesNoLook(Archetype archetype, uint32_t count)
{
	int size = count / poolSize;
	for (int i = 0; i < size; i++)
	{
		CreatePool(archetype);
	}
}

Entity* ecs::ECS::CreateEntities(Archetype archetype, uint32_t count)
{

	//allocate
	//build pools
	//create entities
	return nullptr;
}

Entity ecs::ECS::CreateEntity(Archetype archetype)
{
	int selectedPool = -1;
	if (pools.size() == 0)
	{
		CreatePool(archetype);
		selectedPool = pools.size() - 1;
	}

	for (int i = 0; i < pools.size(); i++)
	{
		if (pools[i].type == archetype)
		{
			if (pools[i].HasRoom())
			{
				selectedPool = i;
				break;
			}
		}
	}

	if (selectedPool == -1)
	{
		CreatePool(archetype);
		selectedPool = pools.size() - 1;
	}
	auto entity = pools[selectedPool].CreateEntity(archetype, selectedPool);

	return entity;
}

void ecs::ECS::RemoveEntity(Entity e)
{
	pools[e.poolId].RemoveEntity(e);
}

void ecs::ECS::ComponentArray::_free()
{
	free(data);
	data = nullptr;
	allocations = 0;
	used = 0;
}

void ecs::ECS::ComponentArray::defrag()
{
}
