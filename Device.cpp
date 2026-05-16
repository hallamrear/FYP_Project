#include "pch.h"
#include "Device.h"

GraphicsDevice* GraphicsDevice::instance;

GraphicsDevice::GraphicsDevice()
{
    isInitialised = false;
    WindowWidth = 0;
    WindowHeight = 0;
    m_Window = nullptr;
}

GraphicsDevice::~GraphicsDevice()
{
  
    isInitialised = false;
}

SDL_Window* GraphicsDevice::GetWindow_Impl()
{
    if (m_Window != nullptr)
        return m_Window;
    else
        return nullptr;
}

HRESULT GraphicsDevice::Init(int width, int height)
{
    HRESULT hr = S_OK;

    WindowWidth = width;
    WindowHeight = height;

    SDL_WindowFlags flags = 0;
    m_Window = SDL_CreateWindow("Smoothed Particle Hydrodynamics", width, height, flags);

    if (m_Window)
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

SDL_Window* GraphicsDevice::GetWindow()
{
    return Get()->GetWindow_Impl();
}

bool GraphicsDevice::GetIsInitialised()
{
    return isInitialised;
}