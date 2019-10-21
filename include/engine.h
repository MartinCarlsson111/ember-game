#include "int2.h"
#include <stdint.h>
#include "SDL2/SDL.h"
#include "glm/glm.hpp"
#undef main

#include "..\include\ecs.h"
#include "renderer.h"

class Engine
{
public:
	Engine(const char* appName, const int2& windowPos, const int2& windowSize, uint32_t flags);


	~Engine();


	void Run();

	static bool isRunning;
	static glm::ivec4 wr;
private:
	Engine()
	{
		window = nullptr;
		surface = nullptr;
		renderer = nullptr;
	}
	const double frameTime = 1.0 / 60.0;
	double deltaTime = 0;

	SDL_Window* window;
	SDL_Surface* surface;
	Renderer* renderer;

};