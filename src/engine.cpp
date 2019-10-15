#include "..\include\engine.h"
#include "..\include\input.h"
#include "..\include\ecs.h"
#include "..\include\componentList.h"
#include "SDL2/sdl.h"
#include "renderSystem.h"
#include "SDL2/SDL_syswm.h"
#undef event
typedef SDL_Event Event;

glm::ivec4 Engine::wr = glm::vec4();

bool Engine::isRunning = false;
Engine::Engine(const char* appName, const int2& windowPos, const int2& windowSize, uint32_t flags)
{
	wr.x = windowPos.x;
	wr.y = windowPos.y;
	wr.z = windowSize.x;
	wr.w = windowSize.y;

	window = SDL_CreateWindow(appName, windowPos.x, windowPos.y, windowSize.x, windowSize.y, flags);
	surface = SDL_GetWindowSurface(window);
	renderer = new Renderer(window);

	std::srand(50035);

	isRunning = true;
}

Engine::~Engine()
{
	delete renderer;
	renderer = nullptr;
	SDL_DestroyWindow(window);
}

void Engine::Run()
{
	ecs::ECS ecs = ecs::ECS();
	auto archetype = ecs.CreateArchetype<Position, Scale, Rotation, Tile, Renderable, Dynamic>();

	uint32_t count = 500000;
	for (size_t i = 0; i < count; i++)
	{
		auto e = ecs.CreateEntity(archetype);

		Position p = Position(i % 64, i / 64);
		ecs.SetComponent<Position>(e, p);

		Scale scale = Scale(0.5f, 0.5f);

		ecs.SetComponent<Scale>(e, scale);

		Tile t = Tile(std::rand() % 255);
		ecs.SetComponent<Tile>(e, t);
	}

	RenderSystem renderSystem = RenderSystem();

	while (isRunning)
	{
		Input::Update();
		Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			switch (event.type)
			{
			case SDL_EventType::SDL_WINDOWEVENT:
			{
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_MAXIMIZED:
				{
					SDL_GetWindowSize(window, &wr.z, &wr.w);
					SDL_GetWindowPosition(window, &wr.x, &wr.y);
					glViewport(wr.x, wr.y, wr.z, wr.w);
				}break;

				case SDL_WINDOWEVENT_RESIZED:
				{
					wr.z = event.window.data1;
					wr.w = event.window.data2;
					glViewport(wr.x, wr.y, wr.z, wr.w);
				}break;
				case SDL_WINDOWEVENT_MOVED:
				{
					wr.x = event.window.data1;
					wr.y = event.window.data2;
					glViewport(wr.x, wr.y, wr.z, wr.w);
				}break;
				}
			}break;

			case SDL_EventType::SDL_MOUSEWHEEL:
			{
				Input::SetMouseWheel(event.wheel.y);
			}break;

			case SDL_EventType::SDL_QUIT:
			{
				isRunning = false;
			}break;
			default:
			{
			}
			}
		}
		renderSystem.Update(&ecs, renderer);
		renderer->Update(0.016f);
		renderer->Render();
		if (Input::GetKeyDown(KeyCode::ESC))
		{
			isRunning = false;
		}
		if (Input::GetKeyDown(KeyCode::ENTER))
		{
			SDL_MaximizeWindow(window);
			SDL_GetWindowSize(window, &wr.z, &wr.w);
			SDL_GetWindowPosition(window, &wr.x, &wr.y);
			glViewport(wr.x, wr.y, wr.z, wr.w);
		}
	}
}
