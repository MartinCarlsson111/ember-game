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
	SpriteBatch debugBatch;

	Shader shader;
	Shader debugShader;

	Texture2DHandler textureHandler;
	Texture2D texture;
	Texture2D debugTexture;

	Archetype staticTiles;
	Archetype dynamicTiles;
	Archetype aabbRenderables;

	std::vector<SpriteVertex> vertices;
	std::vector<SpriteVertex> staticVertices;
	std::vector<SpriteVertex> debugVertices;

	bool debugEnabled;

public:
	RenderSystem(ecs::ECS* ecs);
	~RenderSystem();
	void Update(ecs::ECS* ecs, Renderer* renderer);

	void StaticBatch(ecs::ECS* ecs);
	void DynamicBatch(ecs::ECS* ecs);
	void DebugBatch(ecs::ECS* ecs);

	void InitializeStatic(ecs::ECS* ecs);
	void InitializeDynamic(ecs::ECS* ecs);
	void InitializeDebug(ecs::ECS* ecs);
};