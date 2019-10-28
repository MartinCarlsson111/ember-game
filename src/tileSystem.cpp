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

void TileSystem::LoadMap(const char* filePath, ecs::ECS* ecs)
{
	auto dynamic = ecs->CreateArchetype<Position, Scale, Rotation, Tile, Renderable, Dynamic, AABB>();
	auto stat = ecs->CreateArchetype<Position, Scale, Rotation, Tile, AABB, Renderable, Static>();
	using namespace tinyxml2;

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
							auto e = ecs->CreateEntity(stat);

							Position p = Position(y - chunkX+1, x - chunkY+1);
							ecs->SetComponent<Position>(e, p);	

							Scale scale = Scale(0.5f, 0.5f);
							ecs->SetComponent<Scale>(e, scale);

							Tile t = Tile(id);
							ecs->SetComponent<Tile>(e, id - 1);

							AABB aabb = AABB();
							aabb.h = 1;
							aabb.w = 1;
							aabb.collisionMask = stat.types();

							ecs->SetComponent<AABB>(e, aabb);
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

TileSystem::TileData TileSystem::Query(const int& x, const int& y) const
{
	return TileData();
}
