#pragma once
#include "stdint.h"
#include <vector>
#include <algorithm>
struct Interval
{
	Interval(uint32_t start, uint32_t end) :start(start), end(end) {}
	uint32_t start;
	uint32_t end;

	static const int max = UINT32_MAX;
};

struct IntervalSet
{
	IntervalSet()
	{
		intervals.push_back(Interval(0, Interval::max));
	}

	IntervalSet(uint32_t max)
	{
		intervals.push_back(Interval(0, max));
	}
	IntervalSet(uint32_t start, uint32_t max)
	{
		intervals.push_back(Interval(start, max));
	}

	std::vector<Interval> intervals;
	uint32_t bestInterval = 0;

	void GetBestInterval()
	{
		uint32_t best = UINT32_MAX;
		for (int i = 0; i < intervals.size(); i++)
		{
			if (intervals[i].start < best)
			{
				best = intervals[i].start;
			}
		}
	}

	void FixIntervals()
	{
		intervals.erase(std::remove_if(intervals.begin(), intervals.end(),
			[](Interval v) { return v.start == UINT32_MAX; }), intervals.end());
	}

	uint32_t GetFirst()
	{
		auto retVal = intervals[bestInterval].start;
		intervals[bestInterval].start++;
		if (intervals[bestInterval].start == intervals[bestInterval].end)
		{
			intervals.erase(intervals.begin());
			GetBestInterval();
		}
		return retVal;
	}

	void Add(uint32_t v)
	{
		for (int i = 0; i < intervals.size(); i++)
		{
			if (intervals[i].start == v && intervals[i].end > v)
			{
				return;
			}
			if (intervals[i].start-1 == v)
			{
				intervals[i].start--;
				for (int y = 0; y < intervals.size(); y++)
				{
					if (y == i) continue;
					if (intervals[y].end == intervals[i].start)
					{
						intervals[i].start = intervals[y].start;
						intervals[y].start = UINT32_MAX;
					}
				}
				FixIntervals();
				return;
			}
			else if (intervals[i].end == v - 1)
			{
				intervals[i].end++;
				for (int y = 0; y < intervals.size(); y++)
				{
					if (y == i) continue;
					if (intervals[y].start == intervals[i].end)
					{
						intervals[i].end = intervals[y].end;
						intervals[y].start = UINT32_MAX;
					}
				}
				FixIntervals();
				return;
			}
		}
		intervals.push_back(Interval(v, v + 1));
	}
};

