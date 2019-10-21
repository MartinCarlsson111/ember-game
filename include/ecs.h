#pragma once
#include "entityManager.h"
#include "entity.h"
#include "archetype.h"
#include "component.h"
#include <utility>
#include <type_traits>
#include <vector>
#include "pool.h"
#include "globals.h"

namespace ecs
{
	struct ArrayIntervals
	{
		ArrayIntervals(int index, int count, Pool* pool)
			:index(index), count(count), pool(pool)
		{

		}
		int count = 0;
		int index = 0;
		Pool* pool;
	};
	class ECS
	{
	public:
		ECS();
		~ECS();

	private:
		struct ComponentArray
		{
			void* data = nullptr;
			uint64_t allocations = 0;
			uint64_t used = 0;
			uint16_t typeSize = 0;

			void allocre(uint32_t alloc);

			template<typename T>
			void alloc(uint32_t alloc);
			void _free();
			void defrag();
		};

		uint32_t findLowestSizeHint(Archetype archetype);
		void CreatePool(Archetype type);

		const uint32_t poolSize = 16000U;

		uint32_t hints[MAXCOMPONENTS] = { 0 };
		ComponentArray components[MAXCOMPONENTS];
		std::vector<Pool> pools;
	public:
		template <typename T>
		struct ComponentData
		{
			ComponentData()
			{

			}

			void init(std::vector<ecs::ArrayIntervals> intervals, ComponentArray* compArray)
			{
				auto type = ComponentType::id<T>();
				int size = 0;
				for (int i = 0; i < intervals.size(); i++)
				{
					size += intervals[i].count;
				}
				comps = std::vector<T>(size);
				entities = std::vector<Entity>(size);
				int lastSize = 0;
				for (int i = 0; i < intervals.size(); i++)
				{
					ComponentArray arr = compArray[type];
					T* dataArr = (T*)arr.data;
					memcpy(&comps[lastSize], &dataArr[intervals[i].index], intervals[i].count * sizeof(T));
					memcpy(&entities[lastSize], intervals[i].pool->GetEntities(), intervals[i].count * sizeof(Entity));
					lastSize += intervals[i].count;
				}
			}
			std::vector<T> comps;
			std::vector<Entity> entities;
		private:
		};

		template <typename... Ts>
		typename std::enable_if<sizeof...(Ts) == 0>::type CreateArchetypeA(Archetype& archetype) {
		}

		template <typename T, typename... Ts>
		void CreateArchetypeA(Archetype& archetype) {

			uint32_t id = ComponentType::id<T>();
			if (components[id].data == NULL)
			{
				uint32_t lowestHint = findLowestSizeHint(archetype);
				uint32_t allocSize = lowestHint > poolSize ? poolSize : lowestHint;
				components[id].alloc<T>(allocSize);
			}
			archetype.set(id);
			CreateArchetypeA<Ts...>(archetype);
		}
			
		template<typename... Ts>
		Archetype CreateArchetype()
		{
			Archetype archetype;
			CreateArchetypeA<Ts...>(archetype);
			return archetype;
		}

		void CreateEntitiesNoLook(Archetype archetype, uint32_t count);

		template<typename T>
		void PreAlloc(uint32_t count);

		Entity* CreateEntities(Archetype archetype, uint32_t count);

		template<typename T>
		void HintSize(uint32_t size);

		Entity CreateEntity(Archetype archetype);

		void RemoveEntity(Entity e);

		template<typename First, typename ...T>
		void RemoveComponents(Entity e);

		template <typename T>
		void RemoveComponent(Entity e);

		template <typename T>
		void SetComponent(Entity e, T value);

		template <typename T>
		ComponentData<T> GetComponents();

		template <typename T>
		ComponentData<T> GetComponents(Archetype archetype);

		template <typename T>
		T GetComponent(Entity e);
	};


	template<typename T>
	inline void ECS::PreAlloc(uint32_t count)
	{
		components[ComponentType::id<T>()].alloc<T>(count);
	}

	template<typename T>
	inline void ECS::HintSize(uint32_t size)
	{
		hints[ComponentType::id<T>()] = size;
	}



	template<typename First, typename ...T>
	inline void ECS::RemoveComponents(Entity e)
	{
		//create new archetype
	}
	template<typename T>
	inline void ECS::RemoveComponent(Entity e)
	{
		//entity archetype needs to change
		//get all the component data for the entity
		//pools[e.poolId].RemoveEntity(e);
		//find appropriate pool
		//create entity
		//move component data
		//clear old component data
	}

	template<typename T>
	inline void ECS::SetComponent(Entity e, T value)
	{
		auto poolIndex = pools[e.poolId].GetIndex<T>();
		auto index = poolIndex + e.index();
		T* comp = (T*)components[ComponentType::id<T>()].data;
		comp[index] = value;
	}

	template<typename T>
	inline ECS::ComponentData<T> ECS::GetComponents()
	{
		ComponentData<T> data;
		data.size = components[ComponentType::id<T>()].allocations;
		data.data = (T*)components[ComponentType::id<T>()].data;
		return data;
	}
	template<typename T>
	inline ECS::ComponentData<T> ECS::GetComponents(Archetype archetype)
	{
		std::vector<ArrayIntervals> intervals;
		for (auto pool : pools)
		{
			if (pool.type.has(archetype))
			{
				intervals.push_back(ArrayIntervals(pool.GetIndex<T>(), pool.GetUsed(), &pool));
			}
		}
		auto d = ComponentData<T>();
		d.init(intervals, components);
		return d;
	}

	template<typename T>
	inline T ECS::GetComponent(Entity e)
	{
		int index = pools[e.poolId].GetIndex<T>() + e.index();
		T* comp = (T*)components[ComponentType::id<T>()].data;
		return (T)comp[index];
	}

	inline void ECS::ComponentArray::allocre(uint32_t alloc)
	{
		void* rea = realloc(data, (alloc + allocations) * typeSize);
		if (rea != nullptr)
		{
			data = rea;
			allocations += alloc;
		}
		else
		{
			allocations = 0;
			free(data);
		}
	}
	template<typename T>
	inline void ECS::ComponentArray::alloc(uint32_t alloc)
	{
		typeSize = sizeof(T);
		if (data == nullptr)
		{
			data = (T*)calloc(alloc, typeSize);
		}
	}
}