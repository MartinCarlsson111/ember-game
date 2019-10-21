#include "ecs.h"
#include "renderer.h"
#include <memory>
#include <vector>

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "componentList.h"
class RenderSystem
{
	SpriteBatch batch;
	SpriteBatch dynamicBatch;
	Shader shader;
	Texture2DHandler textureHandler;
	Texture2D texture;
	Archetype staticTiles;
	Archetype dynamicTiles;
	
	class GatherTiles
	{
	private:
		ecs::ECS::ComponentData<Position> positions;

		ecs::ECS::ComponentData<Tile> tiles;
		ecs::ECS::ComponentData<Scale> scales;
		void foo(int index)
		{
			//vertices[index].data = positions;
			//vertices[index].pos = glm::vec2(0);
		}

	public:
		std::vector<SpriteVertex> vertices;
		GatherTiles(ecs::ECS::ComponentData<Position> pos, ecs::ECS::ComponentData<Tile> tiles, ecs::ECS::ComponentData<Scale> scale)
		{
			positions = pos;
			scales = scale;
			this->tiles = tiles;
			//vertices = std::vector<SpriteVertex>(pos.size);
		}

		void operator()(const tbb::blocked_range<size_t>& r) {
			for (size_t i = r.begin(); i != r.end(); ++i)
			{
				foo(i);
			}
		}
	};

public:
	RenderSystem();
	~RenderSystem();
	void Update(ecs::ECS* ecs, Renderer* renderer);
	void Start();

};