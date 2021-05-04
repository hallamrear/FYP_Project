#include "PCH.h"
#include "Renderer.h"
#include "Particle.h"

Renderer* Renderer::instance;

Renderer::Renderer()
{
    WindowHeight = 0;
    WindowWidth = 0;
    isInitialised = false;
}

Renderer::~Renderer()
{
   



}

HRESULT Renderer::Init(int width, int height)
{
    HRESULT hr = E_FAIL;

    WindowWidth = width;
    WindowHeight = height;

    if(GraphicsDevice::Get()->GetIsInitialised() == false)
        GraphicsDevice::Get()->Init(width, height);

    hr = S_OK;

    return hr;
}

Renderer* Renderer::Get()
{
    if (instance == nullptr)
        instance = new Renderer();

    return instance;
}

void Renderer::PrepareFrame()
{
    GraphicsDevice::GetWindow()->clear(sf::Color::Black);
}

void Renderer::PresentFrame()
{
    GraphicsDevice::GetWindow()->display();
}

void Renderer::Render_Impl(sf::Shape* shape)
{
    GraphicsDevice::GetWindow()->draw(*shape);
}

void Renderer::Render_Impl(Particle* particle)
{
    sf::CircleShape shape;

    float r = particle->GetColliderRadius();

    shape.setPosition(particle->GetModel()->position.x - (r/2), particle->GetModel()->position.y - (r));
    shape.setRadius(r);
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color::Red);
    shape.setFillColor(sf::Color::Transparent);

    GraphicsDevice::GetWindow()->draw(shape);
}

void Renderer::Render(sf::Shape* shape)
{
    Get()->Render_Impl(shape);
}

void Renderer::Render(Particle* particle)
{
    Get()->Render_Impl(particle);
}
