#include "engine.h"

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif 

#include "stb_image_write.h"
#include "tbb/tbb.h"
int width = 800;
int height = 600;

int main()
{
	tbb::task_scheduler_init init(12);  // Explicit number of threads

	Engine engine("ember-game", int2(0, 0), int2(width, height), SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	engine.Run();
	CloseWindow(GetConsoleWindow());
	return 0;
}