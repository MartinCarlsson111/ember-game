#include "ecs.h"
#include "renderer.h"
#include <memory>
class RenderSystem
{


	SpriteBatch batch;
	SpriteBatch dynamicBatch;

	Shader shader;
	Texture2DHandler textureHandler;
	Texture2D texture;

	Archetype staticTiles;
	Archetype dynamicTiles;


public:
	~RenderSystem();
	void Update(ecs::ECS* ecs, Renderer* renderer);
	void Start();

};