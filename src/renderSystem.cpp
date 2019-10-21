#include "renderSystem.h"
#include "componentList.h"
#include <iostream>

#include <algorithm>

RenderSystem::RenderSystem()
{
	textureHandler.LoadTextureNoFlip("tileset.png", texture);
	shader.LoadShaders("basic.vert", "basic.frag", "quad.geo");
}

RenderSystem::~RenderSystem()
{
	if (shader.handle != 0)
	{
		shader.DeleteProgram();
		shader.handle = 0;
		textureHandler.FreeTexture(texture);

		glDeleteBuffers(1, &batch.vbo);
		glDeleteBuffers(1, &dynamicBatch.vbo);

		glDeleteVertexArrays(1, &batch.vao);
		glDeleteVertexArrays(1, &dynamicBatch.vao);
	}
}

void RenderSystem::Update(ecs::ECS* ecs, Renderer* renderer)
{
	if (staticTiles == Archetype())
	{
		staticTiles = ecs->CreateArchetype<Position, Scale, Rotation, Tile, Renderable, Static>();

		auto pos = ecs->GetComponents<Position>(staticTiles);
		auto tiles = ecs->GetComponents<Tile>(staticTiles);
		auto scales = ecs->GetComponents<Scale>(staticTiles);

		//GatherTiles spriteVertex = GatherTiles(pos, tiles, scales);

		//tbb::parallel_for(tbb::blocked_range<size_t>((size_t)0, pos.comps.size()), spriteVertex);
		std::vector<SpriteVertex> vertices =  std::vector<SpriteVertex>(pos.comps.size());

		for (int i = 0; i < vertices.size(); i++)
		{
			vertices[i] = SpriteVertex(glm::vec2(pos.comps[i].x, pos.comps[i].y), glm::vec4(scales.comps[i].x, scales.comps[i].y, tiles.comps[i].index, 0));
		}

		if (vertices.size() == 0)
		{
			batch.count = 0;
		}
		else
		{
			if (batch.vbo == 0)
			{ 
				glGenBuffers(1, &batch.vbo);
				glBindBuffer(GL_ARRAY_BUFFER, batch.vbo);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * (sizeof(SpriteVertex)), &vertices[0], GL_STREAM_DRAW);
				glGenVertexArrays(1, &batch.vao);
				glBindVertexArray(batch.vao);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (sizeof(SpriteVertex)), NULL); //Pos
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (sizeof(SpriteVertex)), (GLvoid*)(2 * sizeof(GLfloat)));
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);

				float tileWidth = 256;
				float width = 1.0f / 8191 * tileWidth;

				batch.textureData = glm::vec4(width, width, 32, 32);
				batch.textureHandle = texture.textureHandle;
				batch.shaderHandle = shader.handle;
			}
			else
			{
				glBindBuffer(GL_ARRAY_BUFFER, batch.vbo);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SpriteVertex), &vertices[0], GL_STREAM_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			batch.count = vertices.size();
		}
	}

	dynamicTiles = ecs->CreateArchetype<Position, Scale, Rotation, Tile, Renderable, Dynamic>();

	auto pos = ecs->GetComponents<Position>(dynamicTiles);
	auto tiles = ecs->GetComponents<Tile>(dynamicTiles);
	auto scale = ecs->GetComponents<Scale>(dynamicTiles);
	std::vector<SpriteVertex> vertices = std::vector<SpriteVertex>(pos.comps.size());

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i] = SpriteVertex(glm::vec2(pos.comps[i].x, pos.comps[i].y), glm::vec4(scale.comps[i].x, scale.comps[i].y, tiles.comps[i].index, 0));
	}


	

	if (vertices.size() == 0)
	{
		dynamicBatch.count = 0;
	}
	else
	{
		if (dynamicBatch.vbo == 0)
		{
			glGenBuffers(1, &dynamicBatch.vbo);
			glBindBuffer(GL_ARRAY_BUFFER, dynamicBatch.vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * (sizeof(SpriteVertex)), &vertices[0], GL_STREAM_DRAW);
			glGenVertexArrays(1, &dynamicBatch.vao);
			glBindVertexArray(dynamicBatch.vao);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (sizeof(SpriteVertex)), NULL); //Pos
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (sizeof(SpriteVertex)), (GLvoid*)(2 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			float tileWidth = 16;
			float width = 1.0f / 511.0f * tileWidth;

			dynamicBatch.textureData = glm::vec4(width, width, 32, 32);
			dynamicBatch.textureHandle = texture.textureHandle;
			dynamicBatch.shaderHandle = shader.handle;
		}
		else
		{
			glBindBuffer(GL_ARRAY_BUFFER, dynamicBatch.vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SpriteVertex), &vertices[0], GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		dynamicBatch.count = vertices.size();
	}
	renderer->AddSpriteBatch(dynamicBatch);
	renderer->AddSpriteBatch(batch);
}

void RenderSystem::Start()
{
}
