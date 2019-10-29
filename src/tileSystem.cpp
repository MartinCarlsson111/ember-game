#include "tileSystem.h"
#include "componentList.h"
#include "tinyxml2.h"

TileSystem::TileSystem(const int& tileWidth, const int& tileHeight)
{
}

TileSystem::TileSystem()
{
}

TileSystem::~TileSystem()
{
}

void TileSystem::LoadMap(const char* filePath, const char* tileSetPath, ecs::ECS* ecs)
{
	auto dynamic = ecs->CreateArchetype<Position, Scale, Rotation, Tile, Renderable, Dynamic, AABB>();
	auto staticCollider = ecs->CreateArchetype<Position, Scale, Rotation, Tile, AABB, Renderable, Static>();
	auto tileStatic = ecs->CreateArchetype<Position, Scale, Rotation, Tile, Renderable, Static>();
	using namespace tinyxml2;
	
	{
		XMLDocument tilesetDoc;
		tilesetDoc.LoadFile(tileSetPath);
		XMLNode* rootNode = tilesetDoc.FirstChildElement();

		if (rootNode != nullptr)
		{
			auto nextNode = rootNode->FirstChildElement();
			while (nextNode != nullptr)
			{
				auto it = nextNode->Value();
				if (!strcmp(it, "tile"))
				{
					int gid = nextNode->IntAttribute("id", -1);
					const char* type;
					nextNode->QueryStringAttribute("type", &type);
					tileDB.SetTile(gid+1, TileData(type));
				}
				nextNode = nextNode->NextSiblingElement();
			}
		}
	}



	XMLDocument doc;
	doc.LoadFile(filePath);
	XMLNode* rootNode = doc.FirstChildElement();
	if (rootNode == nullptr)
	{
		return;
	}

	auto nextNode = rootNode->FirstChildElement();
	while (nextNode != nullptr)
	{
		if (!strcmp(nextNode->Value(), "layer"))
		{
			auto dataNode = nextNode->FirstChild();
			auto chunkNode = dataNode->FirstChildElement();
			while (chunkNode != nullptr)
			{
				auto chunkX = chunkNode->IntAttribute("x");
				auto chunkY = chunkNode->IntAttribute("y");
				auto chunkWidth = chunkNode->IntAttribute("width");
				auto chunkHeight = chunkNode->IntAttribute("height");

				auto child = chunkNode->FirstChildElement();

				for (int x = chunkWidth - 1; x >= 0; x--)
				{
					for (int y = chunkHeight - 1; y >= 0; y--)
					{
						auto id = child->IntAttribute("gid", -1);
						if (id != -1 && id < 992)
						{

							TileData data;
							if (tileDB.GetTile(id-1, data))
							{
								if (data.collider && data.dynamic)
								{
									auto e = ecs->CreateEntity(dynamic);

									Position p = Position(y - chunkX + 1, x - chunkY + 1);
									ecs->SetComponent<Position>(e, p);

									Scale scale = Scale(0.5f, 0.5f);
									ecs->SetComponent<Scale>(e, scale);

									ecs->SetComponent<Tile>(e, id - 1);

									AABB aabb = AABB();
									aabb.isStatic = false;
									aabb.h = 1;
									aabb.w = 1;
									aabb.collisionMask = dynamic.types();

									ecs->SetComponent<AABB>(e, aabb);
								}
								else if (data.collider)
								{
									auto e = ecs->CreateEntity(staticCollider);

									Position p = Position(y - chunkX + 1, x - chunkY + 1);
									ecs->SetComponent<Position>(e, p);

									Scale scale = Scale(0.5f, 0.5f);
									ecs->SetComponent<Scale>(e, scale);

									ecs->SetComponent<Tile>(e, id - 1);

									AABB aabb = AABB();
									aabb.isStatic = true;
									aabb.h = 1;
									aabb.w = 1;
									aabb.collisionMask = staticCollider.types();

									ecs->SetComponent<AABB>(e, aabb);
								}
								else
								{
									auto e = ecs->CreateEntity(tileStatic);
									Position p = Position(y - chunkX + 1, x - chunkY + 1);
									ecs->SetComponent<Position>(e, p);
									Scale scale = Scale(0.5f, 0.5f);
									ecs->SetComponent<Scale>(e, scale);
									ecs->SetComponent<Tile>(e, id - 1);
								}
							}
							else
							{
								auto e = ecs->CreateEntity(tileStatic);

								Position p = Position(y - chunkX + 1, x - chunkY + 1);
								ecs->SetComponent<Position>(e, p);
								Scale scale = Scale(0.5f, 0.5f);
								ecs->SetComponent<Scale>(e, scale);
								ecs->SetComponent<Tile>(e, id - 1);
							}
						}
						child = child->NextSiblingElement();
					}
				}
				chunkNode = chunkNode->NextSiblingElement();
			}
		}
		nextNode = nextNode->NextSiblingElement();
	}
}


void TileSystem::TileDatabase::SetTile(int gid, const TileData value)
{
	data[gid-1] = value;
}

bool TileSystem::TileDatabase::GetTile(int id, TileData& tile)
{
	auto it = data.find(id);
	if (it != data.end())
	{
		tile = it->second;
		return true;
	}
	return false;
}

