#include "engine.h"
#include "planetGen.h"


int width = 800;
int height = 600;

int main()
{
	PlanetGen gen;
	gen.Generate(PlanetParameters());

	Engine engine("ember-game", int2(0, 0), int2(width, height), SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	engine.Run();
	return 0;
}



