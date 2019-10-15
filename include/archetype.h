#pragma once
#include <stdint.h>
class Archetype
{
public:
	Archetype()
	{
		componentTypes = 0;
	}

	void set(uint64_t a)
	{
		componentTypes |= 1ULL << a;
	}

	bool has(Archetype other)
	{
		uint64_t result = other.types() & componentTypes;
		return result == other.types();
	}

	bool operator==(Archetype other)
	{
		return this->componentTypes == other.componentTypes;
	}

	uint64_t types() const
	{
		return componentTypes;
	}

private:
	uint64_t componentTypes;

};