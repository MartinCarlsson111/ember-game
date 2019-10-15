#include "renderSystem.h"
#include "componentList.h"
#include <iostream>



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
		std::vector<SpriteVertex> vertices;

		for (auto interval : pos.intervals)
		{
			int size = interval.index + interval.count;
			for (int i = interval.index; i < size; i++)
			{
				vertices.push_back(SpriteVertex(glm::vec2(pos.data[i].x, pos.data[i].y), glm::vec4(0.5f, 0.5f, 1, 0)));
			}
		}

		for (auto interval : tiles.intervals)
		{
			int size = interval.index + interval.count;
			for (int i = interval.index; i < size; i++)
			{
				vertices[i].data.z = tiles.data[i].index;
			}
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

				float tileWidth = 16;
				float width = 1.0f / 512.0f * tileWidth;

				batch.textureData = glm::vec4(width, width, tileWidth, tileWidth);
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
	std::vector<SpriteVertex> vertices;

	auto pos = ecs->GetComponents<Position>(dynamicTiles);
	auto tiles = ecs->GetComponents<Tile>(dynamicTiles);
	auto scale = ecs->GetComponents<Scale>(dynamicTiles);

	for (auto interval : pos.intervals)
	{
		int size = interval.index + interval.count;
		for (int i = interval.index; i < size; i++)
		{
			vertices.push_back(SpriteVertex(glm::vec2(pos.data[i].x, pos.data[i].y), glm::vec4(0.5f, 0.5f, 1, 0)));
		}
	}
	int counter = 0;

	for (auto interval : tiles.intervals)
	{
		int size = interval.index + interval.count;
		for (int i = interval.index; i < size; i++)
		{
			vertices[counter].data.z = tiles.data[i].index;
			counter++;
		}
	}

	counter = 0;

	for (auto interval : scale.intervals)
	{
		int size = interval.index + interval.count;
		for (int i = interval.index; i < size; i++)
		{
			vertices[counter].data.x = scale.data[i].x;
			vertices[counter].data.y = scale.data[i].y;
			counter++;
		}
	}

	if (vertices.size() == 0)
	{
		dynamicBatch.count = 0;
	}
	else
	{
		if (dynamicBatch.vbo == 0)
		{
			shader.LoadShaders("basic.vert", "basic.frag", "quad.geo");
			textureHandler.LoadTextureNoFlip("atlas.png", texture);
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
			float width = 1.0f / 512.0f * tileWidth;

			dynamicBatch.textureData = glm::vec4(width, width, tileWidth, tileWidth);
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
	//renderer->AddSpriteBatch(batch);
}

void RenderSystem::Start()
{
}
