#include "pch.h"
#include "Application.h"
#include "Device.h"
#include "Renderer.h"

#include "Simulation.h"

//TODO : REMOVE
#include <debugapi.h>

Application::Application(float width, float height)
{
	windowWidth = width;
	windowHeight = height;

	isRunning = false;
	isInitialised = false;

	mouseDelta = XMFLOAT2();
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

bool Application::Init(HINSTANCE instance, HWND window)
{
	Win32Instance = instance;
	Win32Window = window;


	if (Renderer::Get()->Init(instance, window, windowWidth, windowHeight) != S_OK)
	{
		isInitialised = false;
		return isInitialised;
	}

	simulation = new Simulation(
		PARTICLE_COUNT,
		PARTICLE_SEARCH_DISTANCE,
		WORLD_SIZE,
		GRID_SIZE);


	keyboardController = std::make_unique<DirectX::Keyboard>();
	mouseController = std::make_unique<DirectX::Mouse>();
	mouseController->SetWindow(window);
	mouseController->SetMode(Mouse::MODE_RELATIVE);

	auto state = mouseController->GetState();

	isInitialised = true;
	return isInitialised;
}

void Application::Shutdown()
{

}

void Application::UpdateMouseInputDetails(int posX, int posY, DWORD flags)
{
	mouseWindowPos.x = posX;
	mouseWindowPos.y = posY;
	mouseFlags = flags;
}

void Application::HandleKeyboardInput(float DeltaTime)
{
	//TODO : implement
	if (GetAsyncKeyState(VK_SPACE))
		simulation->AddParticle(mouseWindowPos);

	if (GetAsyncKeyState(VK_LEFT))
		Renderer::Get()->AlterViewPosition(XMFLOAT2(-100.0f * DeltaTime, 0.0f));

	if (GetAsyncKeyState(VK_RIGHT))
		Renderer::Get()->AlterViewPosition(XMFLOAT2(100.0f * DeltaTime, 0.0f));

	if (GetAsyncKeyState(VK_UP))
		Renderer::Get()->AlterViewPosition(XMFLOAT2(0.0f, 100.0f * DeltaTime));

	if (GetAsyncKeyState(VK_DOWN))
		Renderer::Get()->AlterViewPosition(XMFLOAT2(0.0f, -100.0f * DeltaTime));
}

void Application::HandleMouseInput(float DeltaTime)
{
	//TODO : implement
	std::string str = "WinPosX " + std::to_string(mouseWindowPos.x) + " WinPosY " + std::to_string(mouseWindowPos.y) + '\n';
	OutputDebugStringA(str.c_str());
}

void Application::Update(float DeltaTime)
{
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