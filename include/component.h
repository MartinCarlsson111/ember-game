#pragma once
#include <stdint.h>
//Component ids are assigned at compile time as they are parsed by the compiler
//ComponentType::id<Type>()
class ComponentType
{
	static uint32_t counter;
public:
	template<typename T>
	static uint32_t id()
	{
		static uint32_t _id = counter++;
		return _id;
	}
};