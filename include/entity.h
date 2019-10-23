#pragma once
#include <stdint.h>
#include "archetype.h"
struct Entity
{
public:
	Entity()
	{

	}

	Entity(uint32_t index, uint32_t version, uint64_t type, uint16_t pool) : id(index, version)
	{
		poolId = pool;
		componentMask = type;
	}
	struct Id
	{
		Id() : id(0) {}
		explicit Id(uint64_t id) : id(id) {}
		Id(uint32_t index, uint32_t version){
		  id = (uint64_t)version << 32 | index;

		}
		uint64_t getId() const { return id; }
		uint32_t index() const { return id & 0xffffffffUL; }
		uint32_t version() const { return id >> 32; }
		void incrementVersion()
		{
			uint32_t version = id >> 32;
			version++;
			uint32_t indx = index();
			id = (uint64_t)version << 32 | indx;
		}

		bool operator == (const Id& other) const { return id == other.id; }
		bool operator != (const Id& other) const { return id != other.id; }
		bool operator < (const Id& other) const { return id < other.id; }
	private:
		uint64_t id;
	};

	bool operator == (const Entity& other) const {
		return other.id == id;
	}

	uint32_t version() const
	{
		return id.version();
	}

	uint32_t index() const
	{
		return id.index();
	}

	void incrementVersion()
	{
		id.incrementVersion();
	}

	uint64_t componentMask;
	uint32_t poolId;
private:
	Id id;
};