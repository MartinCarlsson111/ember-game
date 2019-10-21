#pragma once
#include "ecs.h"
class TileSystem
{
public:
	class TileData
	{
		bool collider;
	};

private:
	class TileDatabase
	{

	};

	class TileMap
	{
		//size

	};


	TileDatabase tileDB;
	TileMap tileMap;

public:
	TileSystem(const int& tileWidth, const int& tileHeight);
	TileSystem();
	~TileSystem();
	void LoadMap(const char* filePath, ecs::ECS* ecs);
	TileData Query(const int& x, const int& y) const;
};