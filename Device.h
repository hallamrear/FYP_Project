#pragma once
#include "PCH.h"

class GraphicsDevice
{
private:
	static GraphicsDevice* instance;
	bool isInitialised;
	SDL_Window* m_Window;

	GraphicsDevice();
	~GraphicsDevice();

	SDL_Window* GetWindow_Impl();

public:
	int WindowWidth;
	int WindowHeight;

	HRESULT Init(int width, int height);
	static GraphicsDevice* Get();
	static SDL_Window* GetWindow();

	bool GetIsInitialised();	
};

