#pragma once
#include "globals.h"
#include "archetype.h"
#include "entity.h"
#include "entityManager.h"
#include "component.h"
class Pool
{
public:
	void Init(Archetype type, uint32_t size, uint32_t componentArrayIndex[MAXCOMPONENTS])
	{
		eManager = EntityManager(size);
		this->type = type;
		this->size = size;
		this->componentArrayIndex = std::vector<uint32_t>(componentArrayIndex, componentArrayIndex + MAXCOMPONENTS);
		used = 0;
	}

	void _free()
	{
		eManager._free();
	}

	Archetype type;
	void RemoveEntity(Entity e);
	Entity CreateEntity();

	template<typename T>
	uint32_t GetIndex() const
	{
		auto id = ComponentType::id<T>();
		auto index = componentArrayIndex[id];
		return index;
	}

	bool HasRoom()
	{
		return used < size;
	}

	uint32_t GetUsed() const
	{
		return used;
	}

private:
	uint32_t size = 0;
	uint32_t used = 0;
	EntityManager eManager;
	std::vector<uint32_t> componentArrayIndex;
};