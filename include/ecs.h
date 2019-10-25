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

//TODO: Options
//I want different memory layout for different components
//I need faster accessing
//removing
//Faster setting
//memcpy sucks
//but maybe sucks is better than garbage..
//but how to get contigous arrays?
//better use of SIMD data structures

//entt seems to use similar concept, but std::fill? perf is the same - 20%
//array of pointers to array.......

//only build once, then maintain it...
//increases complexity when deleting/removing
//seems bad


//cache the get arrays
//keep track of when changes have been made




//Components as 
//data storage
//system



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

		const uint32_t poolSize = 12800U;

		uint32_t hints[MAXCOMPONENTS] = { 0 };
		ComponentArray components[MAXCOMPONENTS];
		std::vector<Pool> pools;
	public:

		struct EntityArray
		{
			void Init(std::vector<ecs::ArrayIntervals> intervals)
			{
				int size = 0;
				for (int i = 0; i < intervals.size(); i++)
				{
					size += intervals[i].count;
				}

				entities = std::vector<Entity>(size);
				int lastSize = 0;
				for (int i = 0; i < intervals.size(); i++)
				{
					memcpy(&entities[lastSize], intervals[i].pool->GetEntities(), intervals[i].count * sizeof(Entity));
					lastSize += intervals[i].count;
				}
			}
			std::vector<Entity> entities;
		};
		

		template <typename T>
		struct ComponentArrayP
		{
			T* data;
			int size;
		};

		template <typename T>
		struct ComponentDataWrite
		{
			void Init(std::vector<ecs::ArrayIntervals> intervals, ComponentArray* compArray)
			{
				totalSize = 0;

				for (int i = 0; i < intervals.size(); i++)
				{
					totalSize += intervals[i].count;
				}
				comps = std::vector<ComponentArrayP<T>>();
				comps.resize(intervals.size());

				auto type = ComponentType::id<T>();
				ComponentArray arr = compArray[type];
				T* dataArr = (T*)arr.data;

				for (int i = 0; i < intervals.size(); i++)
				{
					comps[i].data = &dataArr[intervals[i].index];
					comps[i].size = intervals[i].count;
				}
			}
			std::vector<ComponentArrayP<T>> comps;
			int totalSize;
		};


		template <typename T>
		struct ComponentData
		{
			void init(std::vector<ecs::ArrayIntervals> intervals, ComponentArray* compArray)
			{
				auto type = ComponentType::id<T>();
				int size = 0;
				for (int i = 0; i < intervals.size(); i++)
				{
					size += intervals[i].count;
				}

				comps = std::vector<T>();
				comps.resize(size);
				int lastSize = 0;
				ComponentArray arr = compArray[type];
				T* dataArr = (T*)arr.data;
				for (int i = 0; i < intervals.size(); i++)
				{
					memcpy(&comps[lastSize], &dataArr[intervals[i].index], intervals[i].count * sizeof(T));
					lastSize += intervals[i].count;
				}
			}
			std::vector<T> comps;
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
		void GetComponents(Archetype a, ecs::ECS::ComponentData<T>& comp);

		template <typename T>
		void GetComponentsWrite(Archetype a, ecs::ECS::ComponentDataWrite<T>& comp);

		template <typename T>
		T GetComponent(Entity e);

		template <typename T>
		T* GetComponentArray(size_t& size);

		EntityArray GetEntityArray(Archetype a);



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
		int counter = 0;
		for (auto pool : pools)
		{
			if (pool.type.has(archetype))
			{
				intervals.push_back(ArrayIntervals(pool.GetIndex<T>(), pool.GetUsed(), &pools[counter]));
			}
			counter++;
		}
		auto d = ComponentData<T>();
		d.init(intervals, components);
		return d;
	}

	template<typename T>
	inline void ECS::GetComponents(Archetype a, ecs::ECS::ComponentData<T>& comp)
	{
		std::vector<ArrayIntervals> intervals;
		int counter = 0;
		for (auto pool : pools)
		{
			if (pool.type.has(a))
			{
				intervals.push_back(ArrayIntervals(pool.GetIndex<T>(), pool.GetUsed(), &pools[counter]));
			}
			counter++;
		}
		comp.init(intervals, components);
	}

	template<typename T>
	inline void ECS::GetComponentsWrite(Archetype a, ecs::ECS::ComponentDataWrite<T>& comp)
	{
		std::vector<ArrayIntervals> intervals;
		int counter = 0;
		for (auto pool : pools)
		{
			if (pool.type.has(a))
			{
				intervals.push_back(ArrayIntervals(pool.GetIndex<T>(), pool.GetUsed(), &pools[counter]));
			}
			counter++;
		}
		comp.Init(intervals, components);
	}

	template<typename T>
	inline T ECS::GetComponent(Entity e)
	{
		int index = pools[e.poolId].GetIndex<T>() + e.index();
		T* comp = (T*)components[ComponentType::id<T>()].data;
		return (T)comp[index];
	}

	template<typename T>
	inline T* ECS::GetComponentArray(size_t& size)
	{
		auto type = ComponentType::id<T>();
		size = components[type].used;
		return (T*)components[type].data;
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