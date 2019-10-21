#include "..\include\engine.h"
#include "..\include\input.h"
#include "..\include\ecs.h"
#include "..\include\componentList.h"
#include "SDL2/sdl.h"
#include "renderSystem.h"
#include "SDL2/SDL_syswm.h"
#include "tileSystem.h"
#include "..//movementSystem.h"
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
	ecs::ECS* ecs = new ecs::ECS();

	auto dynamic = ecs->CreateArchetype<Position, Scale, Rotation, Tile, Renderable, Static>();
	auto playerarch = ecs->CreateArchetype<Position, Renderable, Tile, Scale, Rotation, Player, Movable, Velocity, AABB, Dynamic>();

	auto player = ecs->CreateEntity(playerarch);

	ecs->SetComponent<Position>(player, Position(0, 0));
	ecs->SetComponent<Tile>(player, Tile(5));
	ecs->SetComponent<Scale>(player, Scale(1.0f, 1.0f));


	//uint32_t count = 100000;
	//for (size_t i = 0; i < count; i++)
	//{
	//	auto e = ecs.CreateEntity(dynamic);

	//	Position p = Position(i % 64, i / 64);
	//	ecs.SetComponent<Position>(e, p);

	//	Scale scale = Scale(0.5f, 0.5f);

	//	ecs.SetComponent<Scale>(e, scale);

	//	Tile t = Tile(std::rand() % 255);
	//	ecs.SetComponent<Tile>(e, t);
	//}
	
	RenderSystem renderSystem = RenderSystem();
	TileSystem tileSystem = TileSystem();
	MovementSystem moveSystem = MovementSystem();
	tileSystem.LoadMap("assets//worldmap//ember-game-map.tmx", ecs);

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
		moveSystem.Move(ecs);
		renderSystem.Update(ecs, renderer);
		renderer->Update(0.016f, ecs);
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
	delete ecs;
	ecs = nullptr;
}
