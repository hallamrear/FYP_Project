#include "pch.h"
#include "Application.h"
#include "Device.h"
#include "Renderer.h"

#include "Simulation.h"
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
	if (GetAsyncKeyState(VK_SPACE))
	{
		simulation->ToggleIsRunning();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && addedThisFrame == false)
	{
		simulation->AddParticle(mouseWindowPos);
		addedThisFrame = true;
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
		simulation->RemoveParticle(mouseWindowPos);
	}
}

void Application::Update(float DeltaTime)
{
	if (addedThisFrame == true)
		addedThisFrame = false;

	sf::Event event;
	while (GraphicsDevice::GetWindow()->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			GraphicsDevice::GetWindow()->close();
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