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

void Renderer::Render_Impl(sf::Vector2f position, sf::Vector2f direction, float length)
{
    sf::Vertex line[] =
    {
        sf::Vertex(position),
        sf::Vertex(sf::Vector2f(position.x + (direction.x * length), position.y + (direction.y * length)))
    };

    GraphicsDevice::GetWindow()->draw(line, 2, sf::Lines);
}

void Renderer::Render_Impl(Particle* particle)
{
    Vector2f dir = Vector2f(particle->GetModel()->velocity);
    dir.GetNormalized();

    sf::Vector2f line = sf::Vector2f(dir.x, dir.y);

    Render_Impl(
        sf::Vector2f(particle->GetModel()->position.x, particle->GetModel()->position.y),
        line,
        1.0f
        );

    sf::CircleShape collider;
    float r = particle->GetColliderRadius();
    collider.setPosition(particle->GetModel()->position.x, particle->GetModel()->position.y);
    collider.setRadius(r);
    collider.setOrigin(r, r);
    collider.setOutlineThickness(2.0f);

    if(particle->GetModel()->IsResting())
        collider.setOutlineColor(sf::Color::Yellow);
    else
        collider.setOutlineColor(sf::Color::Red);

    collider.setFillColor(sf::Color::Transparent);
    GraphicsDevice::GetWindow()->draw(collider);

    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(2.0f, 2.0f));
    rect.setOrigin(sf::Vector2f(1.0f, 1.0f));
    rect.setFillColor(sf::Color::Red);
    rect.setOutlineColor(sf::Color::Red);
    rect.setPosition(particle->GetModel()->position.x, particle->GetModel()->position.y);
    GraphicsDevice::GetWindow()->draw(rect);

    sf::CircleShape search;
    search.setPosition(particle->GetModel()->position.x, particle->GetModel()->position.y);
    search.setRadius(PARTICLE_SEARCH_DISTANCE);
    search.setOrigin(PARTICLE_SEARCH_DISTANCE, PARTICLE_SEARCH_DISTANCE);
    search.setOutlineColor(sf::Color::Green);
    search.setOutlineThickness(1.0f);
    search.setFillColor(sf::Color::Transparent);
    GraphicsDevice::GetWindow()->draw(search);
}

void Renderer::Render(sf::Shape* shape)
{
    Get()->Render_Impl(shape);
}

void Renderer::Render(Particle* particle)
{
    Get()->Render_Impl(particle);
}

void Renderer::Render(sf::Vector2f position, sf::Vector2f direction, float length)
{
    Get()->Render_Impl(position, direction, length);
}

