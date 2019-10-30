#pragma once
#include <chrono>
#include <string>
#include <iostream>
#define PROFILEMS

#ifdef PROFILEMS
const bool report = true;
#else
const bool report = false;
#endif // 


class Profiler
{
public:
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
	using Time = std::chrono::high_resolution_clock;
	using durationCast = decltype(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::nanoseconds()));
	void Start(std::string name, bool report = report)
	{
		this->name = name;
		begin = Time::now();
	}

	void End()
	{
		end = Time::now();
		auto delta = end - begin;
		auto ms = durationCast(delta).count() * 0.000001f;
		begin = end;

		if (report)
		{
			std::cout << name << ms << std::endl;
		}

	}


	void Update()
	{

	}

	void ReportAverage(float time)
	{

	}

	std::string name;
	TimePoint begin;
	TimePoint end;
};