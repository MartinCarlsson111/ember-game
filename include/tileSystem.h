#pragma once
#include "ecs.h"
#include <map>

struct TileData
{
	TileData(bool dynamic, bool collider)
	{
		this->dynamic = dynamic;
		this->collider = collider;
		
	}
	TileData()
	{
		collider = false;
		dynamic = false;
	}

	bool collider;
	bool dynamic;
};

class TileSystem
{
public:


private:
	class TileDatabase
	{
		std::map<int, TileData> data;


	public:
		void SetTile(int gid, const TileData value);
		bool GetTile(int id, TileData& tile);
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
	void LoadMap(const char* filePath, const char* tileSetPath, ecs::ECS* ecs);

};