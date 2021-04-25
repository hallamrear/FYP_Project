#pragma once

class Simulation;

class Application
{
	bool isRunning;
	bool isInitialised;
	float windowWidth;
	float windowHeight;

	XMINT2 mouseWindowPos;
	XMFLOAT2 mouseDelta;
	DWORD mouseFlags;

	Mouse* mouseController;
	Keyboard* keyboardController;

	HINSTANCE Win32Instance;
	HWND Win32Window;

	Simulation* simulation;

public:
	Application(float width, float height);
	~Application();

	bool Init(HINSTANCE hInst, HWND window);
	void Shutdown();

	void UpdateMouseInputDetails(int posX, int posY, DWORD flags);

	void HandleKeyboardInput(float DeltaTime);
	void HandleMouseInput(float DeltaTime);

	bool IsRunning();
	bool IsInitialised();

	void SetRunning(bool state);

	void Update(float DeltaTime);
	void Render();
};

