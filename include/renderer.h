#pragma once
#include <Windows.h>
#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\GL.h"
#include "GL\GLU.h"
#include "glm/glm.hpp"
#pragma comment(lib, "opengl32.lib")

#include "camera.h"
#include "mesh.h"
#include "material.h"
#include <vector>
#include "shader.h"
#include "texture.h"
#include "SDL2/SDL.h"

struct SpriteVertex
{
	//Rotation in radians
	//sX = scale X
	//sY = scale y
	SpriteVertex(float x, float y, float rot, float sX, float sY, int spriteIndex)
	{
		pos.x = x;
		pos.y = y;
		data.x = sX;
		data.y = sY;
		data.z = spriteIndex;
		data.w = rot;
	}

	SpriteVertex(glm::vec2 p, glm::vec4 d) : data(d), pos(p) {

	}
	glm::vec2 pos;
	//xScale, yScale, sprite index, rotation(radians)
	glm::vec4 data;
};

struct SpriteBatch
{
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint count = 0;
	GLuint textureHandle = 0;
	GLuint shaderHandle = 0;
	glm::vec4 textureData;
};


class Renderer
{
	Camera camera;
	std::vector<SpriteBatch> spriteBatches;
public:
	Renderer(SDL_Window* window);
	~Renderer();

	void Render();

	void Update(float dt);

	void AddSpriteBatch(GLuint vao, GLuint vbo, GLuint count, const GLuint textureHandle, const glm::vec4 textureData, const GLuint shaderHandle);
	void AddSpriteBatch(SpriteBatch batch);
private:
	void Clear();
	void Swap();
	SDL_Window* window;
	SDL_GLContext context;
};