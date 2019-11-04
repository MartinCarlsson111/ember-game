#include "renderSystem.h"
#include "componentList.h"
#include <iostream>
#include "input.h"
#include <algorithm>

RenderSystem::RenderSystem(ecs::ECS* ecs)
{
	staticTiles = ecs->CreateArchetype<Position, Scale, Rotation, Tile, Renderable, Static>();
	dynamicTiles = ecs->CreateArchetype<Position, Scale, Rotation, Tile, Renderable, Dynamic>();
	aabbRenderables = ecs->CreateArchetype<Position, AABB>();
	debugEnabled = false;
	textureHandler.LoadTextureNoFlip("..\\assets\\tileset.png", texture);
	textureHandler.LoadTextureNoFlip("..\\assets\\debugTexture.png", debugTexture);
	shader.LoadShaders("..\\shaders\\basic.vert", "..\\shaders\\basic.frag", "..\\shaders\\quad.geo");
	debugShader.LoadShaders("..\\shaders\\basic.vert", "..\\shaders\\basic.frag", "..\\shaders\\quadlines.geo");
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


struct gatherDebugTiles
{
	gatherDebugTiles() {}

	void operator()(const int index, std::vector<SpriteVertex>& vertices, const Position p, const AABB a)
	{
		vertices[index] = SpriteVertex(p.x, p.y, 0, a.w, a.h, (float)a.colliding);
	}
};




void RenderSystem::Update(ecs::ECS* ecs, Renderer* renderer)
{
	if (Input::GetKeyDown(KeyCode::NUM9))
	{
		debugEnabled = !debugEnabled;
	}

	ecs::ECS::ComponentDataWrite<Position> staticPos;
	ecs->GetComponentsWrite<Position>(dynamicTiles, staticPos);

	if (staticPos.totalSize != batch.count && staticPos.totalSize != 0)
	{
		StaticBatch(ecs);
	}
	DynamicBatch(ecs);


	if (debugEnabled)
	{
		DebugBatch(ecs);
	}

	if (batch.count > 0)
	{
		renderer->AddSpriteBatch(batch);
	}
	if (dynamicBatch.count > 0)
	{
		renderer->AddSpriteBatch(dynamicBatch);
	}
	if (debugEnabled)
	{
		if (debugBatch.count > 0)
		{
			renderer->AddSpriteBatch(debugBatch);
		}
	}

}

void RenderSystem::StaticBatch(ecs::ECS* ecs)
{
	static bool initialized = false;
	ecs::ECS::ComponentDataWrite<Position> pos;
	ecs->GetComponentsWrite<Position>(staticTiles, pos);

	ecs::ECS::ComponentDataWrite<Tile> tiles;
	ecs->GetComponentsWrite<Tile>(staticTiles, tiles);

	ecs::ECS::ComponentDataWrite<Scale> scale;
	ecs->GetComponentsWrite<Scale>(staticTiles, scale);

	if (!initialized)
	{
		if (pos.totalSize > 0)
		{
			staticVertices = std::vector<SpriteVertex>(pos.totalSize);
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
							gatherT(j + indexOffset, staticVertices, p.data[j], t.data[j], s.data[j]);
						}
					}
				);
				indexOffset += pos.comps[i].size;
			}
			batch.count = staticVertices.size();
			InitializeStatic(ecs);
			initialized = true;
		}
	}

}

void RenderSystem::DynamicBatch(ecs::ECS* ecs)
{
	ecs::ECS::ComponentDataWrite<Position> pos;
	ecs->GetComponentsWrite<Position>(dynamicTiles, pos);

	ecs::ECS::ComponentDataWrite<Tile> tiles;
	ecs->GetComponentsWrite<Tile>(dynamicTiles, tiles);

	ecs::ECS::ComponentDataWrite<Scale> scale;
	ecs->GetComponentsWrite<Scale>(dynamicTiles, scale);
	static bool initialized = false;
	if (pos.totalSize > 0)
	{
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

		if (!initialized)
		{
			InitializeDynamic(ecs);
			initialized = true;
		}
		else
		{
			glBindBuffer(GL_ARRAY_BUFFER, dynamicBatch.vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SpriteVertex), &vertices[0], GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			dynamicBatch.count = vertices.size();
		}
	}

}

void RenderSystem::DebugBatch(ecs::ECS* ecs)
{
	ecs::ECS::ComponentDataWrite<Position> pos;
	ecs->GetComponentsWrite<Position>(aabbRenderables, pos);

	ecs::ECS::ComponentDataWrite<AABB> aabb;
	ecs->GetComponentsWrite<AABB>(aabbRenderables, aabb);


	static bool initialized = false;


	if (pos.totalSize > 0)
	{
		if (debugVertices.size() < pos.totalSize)
		{
			debugVertices.resize(pos.totalSize);
		}

		int indexOffset = 0;
		for (int i = 0; i < pos.comps.size(); i++)
		{
			gatherDebugTiles gatherT = gatherDebugTiles();
			auto p = pos.comps[i];
			auto a = aabb.comps[i];

			tbb::parallel_for(
				tbb::blocked_range<size_t>(0, p.size),
				[&indexOffset, &gatherT, &p, &a, this](const tbb::blocked_range<size_t>& r) {
					for (size_t j = r.begin(); j < r.end(); ++j)
					{
						gatherT(j + indexOffset, debugVertices, p.data[j], a.data[j]);
					}
				}
			);
			indexOffset += pos.comps[i].size;
		}

		if (!initialized)
		{
			InitializeDebug(ecs);
			initialized = true;
		}
		else
		{
			glBindBuffer(GL_ARRAY_BUFFER, debugBatch.vbo);
			glBufferData(GL_ARRAY_BUFFER, debugVertices.size() * sizeof(SpriteVertex), &debugVertices[0], GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			debugBatch.count = debugVertices.size();
		}
	}
}


void RenderSystem::InitializeStatic(ecs::ECS* ecs)
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

	float tileWidthY = 512;
	float tileWidthX = 256;
	float xWidth = (1.0f / texture.pixelSizeX) * tileWidthX;
	float yWidth = (1.0f / texture.pixelSizeY) * tileWidthY;
	int xTiles = 32;
	int yTiles = 16;
	batch.textureData = glm::vec4(xWidth, yWidth, xTiles, yTiles);
	batch.textureHandle = texture.textureHandle;
	batch.shaderHandle = shader.handle;
	batch.count = staticVertices.size();
}

void RenderSystem::InitializeDynamic(ecs::ECS* ecs)
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

	float tileWidthY = 512;
	float tileWidthX = 256;
	float xWidth = (1.0f / texture.pixelSizeX) * tileWidthX;
	float yWidth = (1.0f / texture.pixelSizeY) * tileWidthY;
	int xTiles = 32;
	int yTiles = 16;
	dynamicBatch.textureData = glm::vec4(xWidth, yWidth, xTiles, yTiles);
	dynamicBatch.textureHandle = texture.textureHandle;
	dynamicBatch.shaderHandle = shader.handle;
	dynamicBatch.count = vertices.size();

}

void RenderSystem::InitializeDebug(ecs::ECS* ecs)
{
	glGenBuffers(1, &debugBatch.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, debugBatch.vbo);
	glBufferData(GL_ARRAY_BUFFER, debugVertices.size() * (sizeof(SpriteVertex)), &debugVertices[0], GL_STREAM_DRAW);
	glGenVertexArrays(1, &debugBatch.vao);
	glBindVertexArray(debugBatch.vao);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (sizeof(SpriteVertex)), NULL); //Pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (sizeof(SpriteVertex)), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	debugBatch.count = debugVertices.size();
	debugBatch.textureHandle = debugTexture.textureHandle;
	debugBatch.shaderHandle = debugShader.handle;
}