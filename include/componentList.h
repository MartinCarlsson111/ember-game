#pragma once
#include <stdint.h>
#include "entity.h"
#include "glm/glm.hpp"

//comps
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


struct Velocity
{
	float x = 0;
	float y = 0;
};

struct Player
{
	uint8_t id = 0;
};

struct Rotation
{
	float theta = 0;
};

struct Renderable
{
	uint16_t texture = 0;
	uint8_t shader = 0;
};

struct Tile
{
	Tile()
	{

	}
	Tile(int index) :index(index)
	{}
	uint16_t index = 0;
};


struct CollisionInfo
{
	Entity collidedWith[MAXNUMBEROFCOLLISIONS];
	float dist = 0;
	uint8_t count = 0;

};

struct AABB
{
	AABB() {}
	AABB(float w, float h, uint64_t mask) :w(w), h(h), collisionMask(mask) {
		this->w = w;
		this->h = h;
	}

	bool isStatic = true;

	float w = 0, h = 0;
	uint64_t collisionMask = 0;



};

struct Speed
{
	float speed;
};

struct Circle
{
	float radius = 0;
};

struct Capsule
{
	float radius = 0;
	float height = 0;
};

struct Polygon
{
	uint16_t index = 0;
};

struct Animation
{
	float time = 0.0f;
	float nextTime = 0.0f;
	uint8_t flags = 0;
	uint16_t index = 0; //index into animation clip array 
	glm::vec4 spriteRect = glm::vec4(0);
}; 


//Flags

struct CollisionStatic
{

};

struct Movable
{

};

struct Static
{

};

struct Dynamic
{

};
