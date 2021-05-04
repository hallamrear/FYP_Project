#pragma once
#include "Vector.h"

class Simulation;

class Application
{
	bool isRunning;
	bool isInitialised;
	float windowWidth;
	float windowHeight;

	Vector2i mouseWindowPos;
	Vector2f mouseDelta;

	HINSTANCE Win32Instance;
	HWND Win32Window;

	Simulation* simulation;

public:
	Application(float width, float height);
	~Application();

	bool Init();
	void Shutdown();

	void UpdateMouseInputDetails(int posX, int posY);

	void HandleKeyboardInput(float DeltaTime);
	void HandleMouseInput(float DeltaTime);

	bool IsRunning();
	bool IsInitialised();

	void SetRunning(bool state);

	void Update(float DeltaTime);
	void Render();
};

