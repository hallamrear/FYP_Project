#include "pch.h"
#include "Device.h"

GraphicsDevice* GraphicsDevice::instance;

GraphicsDevice::GraphicsDevice()
{
    isInitialised = false;
    WindowWidth = 0;
    WindowHeight = 0;
}

GraphicsDevice::~GraphicsDevice()
{
  
    isInitialised = false;
}

sf::RenderWindow* GraphicsDevice::GetWindow_Impl()
{
    if (sfmlWindow != nullptr)
        return sfmlWindow;
    else
        return nullptr;
}

HRESULT GraphicsDevice::Init(int width, int height)
{
    HRESULT hr = S_OK;

    WindowWidth = width;
    WindowHeight = height;

    sfmlWindow = new sf::RenderWindow();
    sfmlWindow->create(sf::VideoMode(width, height), "SPH");

    if (sfmlWindow)
    {
        hr = S_OK;
        isInitialised = true;
    }

	return hr;
}

GraphicsDevice* GraphicsDevice::Get()
{
    if (instance == nullptr)
        instance = new GraphicsDevice();

    return instance;
}

sf::RenderWindow* GraphicsDevice::GetWindow()
{
    return Get()->GetWindow_Impl();
}

bool GraphicsDevice::GetIsInitialised()
{
    return isInitialised;
}