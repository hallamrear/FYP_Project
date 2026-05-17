#include "pch.h"
#include "Application.h"
#include "Device.h"
#include "Renderer.h"

#include "Simulation.h"
#include "Particle.h"

Application::Application(float width, float height)
{
	windowWidth = width;
	windowHeight = height;

	isRunning = false;
	isInitialised = false;

	mouseDelta = Vector2f();
}

Application::~Application()
{

}

bool Application::IsRunning()
{
	return isRunning;
}


bool Application::IsInitialised()
{
	return isInitialised;
}

void Application::SetRunning(bool state)
{
	isRunning = true;
	shiftIsHeld = false;
}

bool Application::Init()
{
	if (Renderer::Get()->Init(windowWidth, windowHeight) != S_OK)
	{
		isInitialised = false;
		return isInitialised;
	}

	simulation = new Simulation(
		MAX_PARTICLE_COUNT,
		WORLD_SIZE,
		GRID_SIZE);

	for (size_t i = 0; i < _countof(Renderer::Get()->ConstantBuffer.Particles); i++)
	{
		Renderer::Get()->ConstantBuffer.Particles[i].x = (float)(rand() % (int)(WORLD_SIZE.x - 100) + 50);
		Renderer::Get()->ConstantBuffer.Particles[i].y = (float)(rand() % (int)(WORLD_SIZE.y - 100) + 50);
	}

	isInitialised = true;
	return isInitialised;
}

void Application::Shutdown()
{

}

void Application::UpdateMouseInputDetails(int posX, int posY)
{
	mouseWindowPos.x = posX;
	mouseWindowPos.y = posY;
}

void Application::HandleKeyboardInput(const SDL_Event& event, float DeltaTime, bool isKeyUpEvent)
{
	switch (event.key.key)
	{
	case SDLK_SPACE:
	{
		if (isKeyUpEvent == false && event.key.repeat == false)
		{
			simulation->ToggleIsRunning();
		}
	}
	break;

	case SDLK_LSHIFT:
	{
		if (isKeyUpEvent == false && event.key.repeat == false)
		{
			simulation->IncrementRenderingDetails();
		}
	}
	break;

	case SDLK_R:
		simulation->ResetSimulation();
		break;

	case SDLK_1:
		simulation->ResetSimulationToExampleOne();
		break;

	case SDLK_2:
		simulation->ResetSimulationToExampleTwo();
		break;

	case SDLK_3:
		simulation->ResetSimulationToExampleThree();
		break;

	case SDLK_Q:
	{
		std::string s;
		s = "Particles: " + std::to_string(mouseWindowPos.x) + '/' + std::to_string(mouseWindowPos.y) + '\n';
		OutputDebugStringA(s.c_str());
	}
	break;

	default:
		break;

	}
}

void Application::HandleMouseInput(float DeltaTime)
{
	SDL_MouseButtonFlags buttonState = SDL_GetMouseState(&mouseWindowPos.x, &mouseWindowPos.y);

	if (buttonState & SDL_BUTTON_LMASK)
	{
		Vector2i pos = { (int)mouseWindowPos.x, (int)mouseWindowPos.y };
		simulation->AddParticle(pos);
	}
	
	if (buttonState & SDL_BUTTON_RMASK)
	{
		Vector2i pos = { (int)mouseWindowPos.x, (int)mouseWindowPos.y };
		Particle* p = simulation->AddParticle(pos);
		p->isStatic = true;
	}
}

void Application::Update(float DeltaTime)
{	
	SDL_Event e{};

	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
			case SDL_EVENT_KEY_UP:
			case SDL_EVENT_KEY_DOWN:
			{
				HandleKeyboardInput(e, DeltaTime, e.type == SDL_EVENT_KEY_UP);
			}
			break;

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			case SDL_EVENT_MOUSE_MOTION:
			{
				HandleMouseInput(DeltaTime);
			}
			break;

			case SDL_EVENT_QUIT:
			{
				isRunning = false;
				return;
			}
			break;
		}
	}

	Renderer::Get()->ConstantBuffer.MousePosition = mouseWindowPos;
	Renderer::Get()->ConstantBuffer.DeltaTime = DeltaTime;
	Renderer::Get()->ConstantBuffer.ElapsedTime += DeltaTime;
	memcpy(&Renderer::Get()->ConstantBuffer.Particles, simulation->GetParticleSystem()->Positions, sizeof(Vector2f) * MAX_PARTICLE_COUNT);

	simulation->Update(DeltaTime);
}

void Application::Render()
{
	Renderer::Get()->ComputePass();

	Renderer::Get()->PrepareFrame();

	simulation->Render();

	Renderer::Get()->PresentFrame();
}