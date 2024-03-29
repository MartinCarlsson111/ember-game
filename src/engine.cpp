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
#include "..//broadphaseSystem.h"
#include <chrono>
#include <iostream>


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

	TileSystem tileSystem = TileSystem();
	tileSystem.LoadMap("..//assets//worldmap//ember-game-map.tmx", "..//assets//worldmap//tileset.tsx", ecs);
	auto playerarch = ecs->CreateArchetype<Position, Renderable, Tile, Scale, Rotation, Player, Movable, Velocity, AABB, Dynamic>();
	auto dynamic = ecs->CreateArchetype<Position, Renderable, Tile, Scale, Rotation, Movable, Velocity, AABB, Dynamic>();
	auto staticArch = ecs->CreateArchetype<Position, Renderable, Tile, Scale, Rotation, AABB, Static>();
	auto player = ecs->CreateEntity(playerarch);

	ecs->SetComponent<Position>(player, Position(0, 0));
	ecs->SetComponent<Tile>(player, Tile(32));
	ecs->SetComponent<Scale>(player, Scale(0.5f, 0.5f));

	AABB aabbPlayer = AABB(0.125f, 0.125f, player.componentMask);
	aabbPlayer.isStatic = false;
	ecs->SetComponent<AABB>(player, aabbPlayer);

	uint32_t countStatic = 0;
	uint32_t targetWorldHeight = 0; 
	uint32_t divisor = 0; 
	if (countStatic > 0)
	{
		targetWorldHeight = std::sqrt(countStatic);
		divisor = countStatic / targetWorldHeight;

		for (int i = 0; i < countStatic; i++)
		{
			auto e = ecs->CreateEntity(staticArch);

			Position p = Position(i % divisor, i / divisor);
			ecs->SetComponent<Position>(e, p);

			Scale scale = Scale(0.5f, 0.5f);
			ecs->SetComponent<Scale>(e, scale);

			Tile t = Tile(11);
			ecs->SetComponent<Tile>(e, t);

			AABB aabb = AABB();
			aabb.h = 1;
			aabb.w = 1;
			aabb.collisionMask = staticArch.types();
			ecs->SetComponent<AABB>(e, aabb);
		}
	}
	
	uint32_t count = 1000;
	if (count != 0)
	{
		divisor = count / std::sqrt(count);
	}
	for (size_t i = 0; i < count; i++)
	{
		auto e = ecs->CreateEntity(dynamic);

		Position p = Position((i % divisor), (i / divisor));
		ecs->SetComponent<Position>(e, p);

		Scale scale = Scale(0.5f, 0.5f);

		ecs->SetComponent<Scale>(e, scale);

		Tile t = Tile(32 + (i % 2));
		ecs->SetComponent<Tile>(e, t);
		ecs->SetComponent<Velocity>(e, Velocity() = { 0, 0/*(std::rand() % 100) * 0.01f, (std::rand() % 100) * 0.01f */ });
		AABB aabb = AABB();
		aabb.h = 0.125f;
		aabb.w = 0.125f;					
		aabb.collisionMask = dynamic.types();
		aabb.isStatic = i % 2;
		ecs->SetComponent<AABB>(e, aabb);
	}
	
	RenderSystem renderSystem = RenderSystem(ecs);
	MovementSystem moveSystem = MovementSystem();
	BroadPhaseSystem broadPhaseSystem = BroadPhaseSystem();
	using namespace std::chrono;

	double frameTimeAccu = 0.0;
	int frameCounter = 0;
	auto begin = high_resolution_clock::now();
	while (isRunning)
	{
		auto end = high_resolution_clock::now();
		auto dt = end - begin;
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
		moveSystem.Move(dt.count() * 0.000000001f, ecs);
		broadPhaseSystem.Run(ecs);
	    renderSystem.Update(ecs, renderer);
		renderer->Update(dt.count() * 0.000000001f, ecs);
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

		auto ms = duration_cast<nanoseconds>(dt).count();
		frameTimeAccu += ms;
		frameCounter++;
		if (frameTimeAccu >= 1000000000.0)
		{
			std::cout << frameCounter << std::endl;
			frameTimeAccu = 0.0;
			frameCounter = 0;
		}
		begin = end;
	}
	delete ecs;
	ecs = nullptr;
}
