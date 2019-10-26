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

struct gatherTiles {
	gatherTiles() {}
	void operator()(const int index, std::vector<SpriteVertex>& vertices, const Position p, const Tile t, const Scale s ) {
		vertices[index] = SpriteVertex(p.x, p.y, 0, s.x, s.y, t.index);
	}
};

void RenderSystem::Update(ecs::ECS* ecs, Renderer* renderer)
{
	if (staticTiles == Archetype())
	{
		staticTiles = ecs->CreateArchetype<Position, Scale, Rotation, Tile, Renderable, Static>();
		ecs::ECS::ComponentData<Position> pos;

		ecs->GetComponents<Position>(staticTiles, pos);
		auto tiles = ecs->GetComponents<Tile>(staticTiles);
		auto scales = ecs->GetComponents<Scale>(staticTiles);

		std::vector<SpriteVertex> staticVertices = std::vector<SpriteVertex>(pos.comps.size());
		gatherTiles gatherT = gatherTiles();
		tbb::parallel_for(
			tbb::blocked_range<size_t>(0, pos.comps.size()),
			[&gatherT, &pos, &tiles, &scales, &staticVertices](const tbb::blocked_range<size_t>& r) {
				for (size_t i = r.begin(); i < r.end(); ++i)
				{
					gatherT(i, staticVertices, pos.comps[i], tiles.comps[i], scales.comps[i]);
				}
			}
		);

		if (staticVertices.size() == 0)
		{
			batch.count = 0;
		}
		else
		{
			if (batch.vbo == 0)
			{ 
				glGenBuffers(1, &batch.vbo);
				glBindBuffer(GL_ARRAY_BUFFER, batch.vbo);
				glBufferData(GL_ARRAY_BUFFER, staticVertices.size() * (sizeof(SpriteVertex)), &staticVertices[0], GL_STREAM_DRAW);
				glGenVertexArrays(1, &batch.vao);
				glBindVertexArray(batch.vao);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (sizeof(SpriteVertex)), NULL); //Pos
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (sizeof(SpriteVertex)), (GLvoid*)(2 * sizeof(GLfloat)));
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);

				float tileWidth = 16;
				float width = 1.0f / 511 * tileWidth;

				batch.textureData = glm::vec4(width, width, 32, 32);
				batch.textureHandle = texture.textureHandle;
				batch.shaderHandle = shader.handle;
			}
			else
			{
				glBindBuffer(GL_ARRAY_BUFFER, batch.vbo);
				glBufferData(GL_ARRAY_BUFFER, staticVertices.size() * sizeof(SpriteVertex), &staticVertices[0], GL_STREAM_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			batch.count = staticVertices.size();
		}
	}

	dynamicTiles = ecs->CreateArchetype<Position, Scale, Rotation, Tile, Renderable, Dynamic>();
	ecs::ECS::ComponentDataWrite<Position> pos;
	ecs->GetComponentsWrite<Position>(dynamicTiles, pos);

	ecs::ECS::ComponentDataWrite<Tile> tiles;
	ecs->GetComponentsWrite<Tile>(dynamicTiles, tiles);

	ecs::ECS::ComponentDataWrite<Scale> scale;
	ecs->GetComponentsWrite<Scale>(dynamicTiles, scale);


	if (vertices.size() < pos.totalSize)
	{
		vertices.resize(pos.totalSize);
	}
	int indexOffset = 0;
	for (int i = 0; i < pos.comps.size(); i++)
	{
		gatherTiles gatherT = gatherTiles();

		auto p = pos.comps[i];
		auto t = tiles.comps[i];
		auto s = scale.comps[i];
		tbb::parallel_for(
			tbb::blocked_range<size_t>(0, p.size),
			[&indexOffset, &gatherT, &p, &t, &s, this](const tbb::blocked_range<size_t>& r) {
				for (size_t j = r.begin(); j < r.end(); ++j)
				{
					gatherT(j + indexOffset, vertices, p.data[j], t.data[j], s.data[j]);
				}
			}
		);
		indexOffset += pos.comps[i].size;
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
			float width = 1.0f / 511 * tileWidth;

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
	renderer->AddSpriteBatch(batch);
	renderer->AddSpriteBatch(dynamicBatch);
}

void RenderSystem::Start()
{
}
