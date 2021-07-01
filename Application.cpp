#include "pch.h"
#include "Application.h"
#include "Device.h"
#include "Renderer.h"

#include "Simulation.h"
#include "Particle.h"
#include "SFML\System.hpp"

//TODO : REMOVE
#include <debugapi.h>

Application::Application(float width, float height)
{
	windowWidth = width;
	windowHeight = height;

	isRunning = false;
	isInitialised = false;

	mouseDelta = Vector2f();

	//Particle Info
	DEFAULT_PARTICLE_MASS = 1.0f;
	PARTICLE_COLLIDER_SIZE = 5.0f;
	PARTICLE_INTERACTION_DISTANCE = 100.0f;

	//Simulation stuff
	STARTING_PARTICLE_COUNT = 1000;
	MAX_PARTICLE_COUNT = 10000;
	KERNEL_HEIGHT = 16.0f;
	GRAVITY = Vector2f(0.0f, 9.81f);
	DAMPENING = -0.75f;
	VISCOSITY_CONSTANT_BETA = 0.1f;
	VISCOSITY_CONSTANT_SIGMA = 0.0f;
	GAS_CONSTANT = 0.04F;
	NEAR_STIFFNESS_PARAM = 0.01F;
	REST_DENSITY = 10.0f;

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

	emitterPosition = Vector2f(WORLD_SIZE.x / 2, WORLD_SIZE.y / 8);

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

void Application::HandleKeyboardInput(float DeltaTime)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && spaceIsHeld == false)
	{
		spaceIsHeld = true;
		simulation->ToggleIsRunning();
	}

	if (spaceIsHeld == true && sf::Keyboard::isKeyPressed(sf::Keyboard::Space) == false)
	{
		spaceIsHeld = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::L) && emitterActive == false)
	{
		emitterActive = true;
	}

	if (emitterActive == true && sf::Keyboard::isKeyPressed(sf::Keyboard::L) == false)
	{
		emitterActive = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
	{
		simulation->ResetSimulation();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
	{
		simulation->ResetSimulationToExample();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
	{
		std::string s;
		s = "Particles: " + std::to_string(simulation->GetParticleSystem()->livingParticleCount) + '/' + std::to_string(simulation->GetParticleSystem()->MaxParticleCount) + '\n';
		OutputDebugStringA(s.c_str());
	}

}

void Application::HandleMouseInput(float DeltaTime)
{
	mouseWindowPos.x = sf::Mouse::getPosition(*GraphicsDevice::GetWindow()).x;
	mouseWindowPos.y = sf::Mouse::getPosition(*GraphicsDevice::GetWindow()).y;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		simulation->AddParticle(mouseWindowPos);
	}
	
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		Particle* p = simulation->AddParticle(mouseWindowPos);
		p->isStatic = true;

		//simulation->RemoveParticle(mouseWindowPos);
	}
}

void Application::Update(float DeltaTime)
{	
	sf::Event event;
	while (GraphicsDevice::GetWindow()->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			isRunning = false;
			GraphicsDevice::GetWindow()->close();
		}
	}

	HandleMouseInput(DeltaTime);
	HandleKeyboardInput(DeltaTime);

	if (emitterActive)
	{
		simulation->AddParticle(Vector2i(emitterPosition.x, emitterPosition.y));
	}

	simulation->Update(DeltaTime);
}

void Application::Render()
{
	Renderer::Get()->PrepareFrame();

	if (emitterActive)
		Renderer::RenderText("Emitter active", 24.0f, Vector2f(50.0f, 50.0f), sf::Color::Blue);

	simulation->Render();

	Renderer::Get()->PresentFrame();
}