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

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && shiftIsHeld == false)
	{
		shiftIsHeld = true;
		simulation->IncrementRenderingDetails();
	}

	if (shiftIsHeld == true && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) == false)
	{
		shiftIsHeld = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
		simulation->ResetSimulation();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
		simulation->ResetSimulationToExampleOne();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
		simulation->ResetSimulationToExampleTwo();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
		simulation->ResetSimulationToExampleThree();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		std::string s;
		s = "Particles: " + std::to_string(mouseWindowPos.x) + '/' + std::to_string(mouseWindowPos.y) + '\n';
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

	simulation->Update(DeltaTime);
}

void Application::Render()
{
	Renderer::Get()->PrepareFrame();

	simulation->Render();

	Renderer::Get()->PresentFrame();
}