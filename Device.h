#pragma once
#include "PCH.h"
#include "SFML\Graphics.hpp"

class GraphicsDevice
{
private:
	static GraphicsDevice* instance;
	bool isInitialised;
	sf::RenderWindow* sfmlWindow;

	GraphicsDevice();
	~GraphicsDevice();

	sf::RenderWindow* GetWindow_Impl();

public:
	int WindowWidth;
	int WindowHeight;

	HRESULT Init(int width, int height);
	static GraphicsDevice* Get();
	static sf::RenderWindow* GetWindow();

	bool GetIsInitialised();	
};

