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
	auto dynamic = ecs->CreateArchetype<Position, Scale, Rotation, Tile, Renderable, Dynamic, AABB, Velocity>();
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
					auto properties = nextNode->FirstChildElement();
					int gid = nextNode->IntAttribute("id", -1);
					if (properties)
					{
						XMLElement* propertyElements = properties->FirstChildElement();
						bool collider = false;
						bool isDynamic = false;
						while (propertyElements != nullptr)
						{
							auto name = propertyElements->Attribute("name");
							if (!strcmp(name, "collider"))
							{
								collider = !strcmp(propertyElements->Attribute("value"), "true") ? true : false;
							}
							if (!strcmp(name, "dynamic"))
							{
								isDynamic = !strcmp(propertyElements->Attribute("value"), "true") ? true : false;
							}
							propertyElements = propertyElements->NextSiblingElement();
						}
						tileDB.SetTile(gid + 1, TileData(isDynamic, collider));
					}
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

	XMLElement* rootElem = rootNode->ToElement();

	float tilew = 1.0f;
	float tileh = 1.0f;
	if (rootElem)
	{
		//tilew = 1.0f / (float)rootElem->IntAttribute("tilewidth", 0);
		//tileh = 1.0f / (float)rootElem->IntAttribute("tileheight", 0);

	}

	float tileWidthHalf = tilew / 4.0f;
	float tileHeightHalf = tileh / 8.0f;

	float xOffset = 10;
	float yOffset = 10;



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
							float xPos = y - chunkX + 1;
							float yPos = x - chunkY + 1;
							TileData data;
							if (tileDB.GetTile(id-1, data))
							{
								if (data.collider && data.dynamic)
								{
									auto e = ecs->CreateEntity(dynamic);

									Position p = Position((xPos - yPos) * tileWidthHalf, (xPos + yPos) * tileHeightHalf);
									p.x += xOffset;
									p.y += yOffset;
									ecs->SetComponent<Position>(e, p);

									Scale scale = Scale(0.5f, 0.5f);
									ecs->SetComponent<Scale>(e, scale);

									ecs->SetComponent<Tile>(e, id - 1);

									ecs->SetComponent<Velocity>(e, Velocity() = { 0, 0 });

									AABB aabb = AABB();
									aabb.isStatic = false;
									aabb.h = 0.125f;
									aabb.w = 0.125f;
									aabb.collisionMask = dynamic.types();

									ecs->SetComponent<AABB>(e, aabb);
								}
								else if (data.collider)
								{
									auto e = ecs->CreateEntity(dynamic);
		

									Position p = Position((xPos - yPos) * tileWidthHalf, (xPos + yPos) * tileHeightHalf);
									p.x += xOffset;
									p.y += yOffset;
									ecs->SetComponent<Position>(e, p);

									Scale scale = Scale(0.5f, 0.5f);
									ecs->SetComponent<Scale>(e, scale);

									ecs->SetComponent<Tile>(e, id - 1);

									AABB aabb = AABB();
									aabb.isStatic = true;
									aabb.h = 0.125f;
									aabb.w = 0.125f;
									aabb.collisionMask = dynamic.types();

									ecs->SetComponent<AABB>(e, aabb);
								}
								else
								{
									auto e = ecs->CreateEntity(tileStatic);

									Position p = Position((xPos - yPos) * tileWidthHalf, (xPos + yPos) * tileHeightHalf);
									p.x += xOffset;
									p.y += yOffset;
									ecs->SetComponent<Position>(e, p);

									Scale scale = Scale(0.5f, 0.5f);
									ecs->SetComponent<Scale>(e, scale);
									ecs->SetComponent<Tile>(e, id - 1);
								}
							}
							else
							{
								auto e = ecs->CreateEntity(tileStatic);

								float xPos = y - chunkX + 1;
								float yPos = x - chunkY + 1;
								
								Position p = Position((xPos - yPos) * tileWidthHalf, (xPos + yPos) * tileHeightHalf);
								p.x += xOffset;
								p.y += yOffset;
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

