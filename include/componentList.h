#pragma once
#include <stdint.h>
struct Position
{
	Position()
	{
		x = 0;
		y = 0;
	}
	Position(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
	float x = 0;
	float y = 0;
};

struct Scale
{
	Scale()
	{

	}

	Scale(float x, float y):x(x), y(y)
	{

	}

	float x = 0;
	float y = 0;
};

struct Movable
{

};

struct Velocity
{
	float x = 0, y = 0;
};

struct Player
{
	int id = 0;
};

struct Dynamic
{

};

struct Rotation
{
	float theta = 0;
};

struct Renderable
{
	int texture = 0;
	int shader = 0;
};

struct Tile
{
	Tile()
	{

	}
	Tile(int index) :index(index)
	{}
	int index = 0;
};

struct Static
{

};

struct AABB
{

};

struct Polygon
{
	uint16_t index;
};