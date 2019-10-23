#pragma once

//Component ids are assigned at compile time as they are parsed by the compiler
//ComponentType::id<Type>()
class ComponentType
{
	static size_t counter;
public:
	template<typename T>
	static size_t id()
	{
		static size_t _id = counter++;
		return _id;
	}
};