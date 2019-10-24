#include "ecs.h"
#include "renderer.h"
#include <memory>
#include <vector>

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "componentList.h"
#include "vertex.h"
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
	public:
		std::vector<SpriteVertex> vertices;
		GatherTiles()
		{
		}

		void operator()() {
		}
	};



public:
	std::vector<SpriteVertex> vertices;
	RenderSystem();
	~RenderSystem();
	void Update(ecs::ECS* ecs, Renderer* renderer);
	void Start();

};