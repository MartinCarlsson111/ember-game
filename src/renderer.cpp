#include "renderer.h"
#include "glm/gtc/type_ptr.hpp"
#include <string>
#include <sstream>
#include <iostream>


Renderer::Renderer(SDL_Window* window)
	:camera(glm::vec3(0,0,0), 45, -1, 1000)
{
	this->window = window;
	context = SDL_GL_CreateContext(window);
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
	}
	glEnable(GL_MULTISAMPLE_ARB);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	SDL_GL_SetSwapInterval(0);
	//gen.Generate(PlanetParameters());
	//gen.shader.LoadShaders("colorShader.vert", "colorShader.frag");
}

Renderer::~Renderer()
{
	SDL_GL_DeleteContext(context);
}

void Renderer::Update(float dt, ecs::ECS* ecs)
{
	camera.Update(dt, ecs);
}

void Renderer::AddSpriteBatch(GLuint vao, GLuint vbo, GLuint count, const GLuint textureHandle, const glm::vec4 textureData, const GLuint shaderHandle)
{
	SpriteBatch batch;
	batch.vao = vao;
	batch.vbo = vbo;
	batch.count = count;
	batch.textureData = textureData;
	batch.textureHandle = textureHandle;
	batch.shaderHandle = shaderHandle;
	spriteBatches.push_back(batch);
}

void Renderer::AddSpriteBatch(SpriteBatch batch)
{
	spriteBatches.push_back(batch);
}

void Renderer::Clear()
{
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void Renderer::Render()
{
	Clear();
	for (auto batch : spriteBatches)
	{
		glUseProgram(batch.shaderHandle);

		unsigned int loc = glGetUniformLocation(batch.shaderHandle, "vp");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(camera.GetView() * camera.GetProjection()));

		loc = glGetUniformLocation(batch.shaderHandle, "tileMapData");

		glUniform4f(loc, batch.textureData.x, batch.textureData.y, batch.textureData.z, batch.textureData.w);

		glActiveTexture(GL_TEXTURE0 + 1);
		loc = glGetUniformLocation(batch.shaderHandle, "albedoMap");
		glUniform1i(loc, batch.textureHandle);
		glBindTexture(GL_TEXTURE_2D, batch.textureHandle);
		glBindVertexArray(batch.vao);

		glDrawArrays(GL_POINTS, 0, batch.count);
	}

	
	//gen.shader.Use();
	//gen.shader.SetUniform("model", glm::mat4(1));
	//gen.shader.SetUniform("view", camera.GetView());
	//gen.shader.SetUniform("projection", camera.GetProjection());
	//glBindVertexArray(gen.mesh.mesh.vao);
	//glDrawArrays(GL_TRIANGLES, 0, gen.mesh.vertices.size());

	Swap();
}

void Renderer::Swap()
{
	SDL_GL_SwapWindow(window);
	spriteBatches.clear();
}	
/*
	for (auto instance : instancedDrawCalls)
	{
		glUseProgram(instance.shader);
		glBindBuffer(GL_ARRAY_BUFFER, quad.mesh.vbo);
		glBindVertexArray(quad.mesh.vao);

		unsigned int loc = glGetUniformLocation(instance.shader, "view");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(camera.GetView()));

		loc = glGetUniformLocation(instance.shader, "projection");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(camera.GetProjection()));

		glDrawArraysInstanced(GL_TRIANGLES, 0, quad.mesh.nVerts, instance.matrices.size());
		//glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	*/
	/*
staticnSprite = countSprites;
glBindBuffer(GL_ARRAY_BUFFER, staticvbo);
//glBufferSubData(GL_ARRAY_BUFFER, 0, ps.size(), &ps[0]);
glBufferData(GL_ARRAY_BUFFER, ps2.size() * sizeof(SpriteVertex), &ps2[0], GL_STREAM_DRAW);
glBindBuffer(GL_ARRAY_BUFFER, 0);

*/	/*
	std::vector<glm::vec2> ps = std::vector<glm::vec2>(10000 * 150);
	for (int x = 0; x < 10000; x++)
	{
		for (int y = 0; y < 150; y++)
		{
			ps.push_back(glm::vec2(x * 1.2f, y * 1.2f));
		}
	}*/

	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, ps.size(), &ps[0]);
	//glBufferData(GL_ARRAY_BUFFER, ps.size() * sizeof(glm::vec2), &ps[0], GL_STREAM_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//nSprites = ps.size();
	/*
	std::vector<SpriteVertex> ps2 = std::vector <SpriteVertex>();
	ps2.reserve(3000 * 150);
	int countSprites = 0;
	for (int x = 0; x < 3000; x++)
	{
		for (int y = 0; y < 150; y++)
		{
			ps2.push_back(SpriteVertex(glm::vec2(-x * 1.2f, -y * 1.2f), glm::vec4(0.5f, 0.5f, counter, 0)));
			countSprites++;
		}
	}*/	//shader.LoadShaders("basic.vert", "basic.frag", "quad.geo");

	//std::vector<SpriteVertex> ps;
	//for (int x = 0; x < 300000; x++)
	//{
	//	for (int y = 0; y < 150; y++)
	//	{
	//		ps.push_back(SpriteVertex(glm::vec2(x * 1.2f, y * 1.2f), glm::vec4(y * 0.01f+ .1f, y * 0.01f + .1f, 1, 0)));
	//	}
	//}
	
	//shader.LoadShaders("basic.vert", "basic.frag", "quad.geo");
	//handler.LoadTextureNoFlip("atlas.png", texture);

	//glGenBuffers(1, &vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, ps.size() * (sizeof(SpriteVertex)), &ps[0], GL_STREAM_DRAW);
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (sizeof(SpriteVertex)), NULL); //Pos
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (sizeof(SpriteVertex)), (GLvoid*)(2 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
	//nSprites = ps.size();

	//glGenBuffers(1, &staticvbo);
	//glBindBuffer(GL_ARRAY_BUFFER, staticvbo);
	//glBufferData(GL_ARRAY_BUFFER, ps.size() * (sizeof(SpriteVertex)), &ps[0], GL_STATIC_DRAW);
	//glGenVertexArrays(1, &staticvao);
	//glBindVertexArray(staticvao);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (sizeof(SpriteVertex)), NULL); // pos
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (sizeof(SpriteVertex)), (GLvoid*)(2 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);