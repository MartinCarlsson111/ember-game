#pragma once
#include "ecs.h"
#include <map>

struct TileData
{
	TileData(const char* type)
	{
		dynamic = false;
		collider = false;
		if (!strcmp(type, "tree"))
		{
			collider = false;
			dynamic = false;
		}
		else if (!strcmp(type, "box"))
		{
			dynamic = true;
			collider = true;
		}
		else if (!strcmp(type, "stone"))
		{
			dynamic = false;
			collider = true;
		}
		else if (!strcmp(type, "ground"))
		{
			dynamic = false;
			collider = true;
		}
		else if (!strcmp(type, "ladder"))
		{
			dynamic = false;
			collider = false;
		}
		else if (!strcmp(type, "sky"))
		{
			dynamic = false;
			collider = false;
		}
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