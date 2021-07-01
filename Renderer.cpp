#include "PCH.h"
#include "Renderer.h"
#include "Particle.h"
#include "Overloads.h"

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

    if (!font.loadFromFile("fonts/arial.ttf"))
    {
        // error...
        OutputDebugStringA("Renderer: FAILED TO LOAD TEXT FROM FILE");
        return E_FAIL;
    }

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
        sf::Vertex(position, sf::Color::Magenta),
        sf::Vertex(sf::Vector2f(position.x + (direction.x * length), position.y + (direction.y * length)), sf::Color::Magenta)
    };

    GraphicsDevice::GetWindow()->draw(line, 2, sf::Lines);
}

void Renderer::RenderLine_Impl(Vector2f start, Vector2f end, float thickness, sf::Color color)
{
    sf::Vector2f dv = sf::Vector2f(end.x - start.x, end.y - start.y);
    float dl = (float)sqrtf(dv.x * dv.x + dv.y * dv.y);
    sf::Vector2f uv = sf::Vector2f(dv.x / dl, dv.y / dl);
    sf::Vector2f up = sf::Vector2f(-uv.y, uv.x);
    sf::Vector2f offset = up * (thickness / 2.0f);

    sf::Vector2f st = sf::Vector2f(start.x, start.y);
    sf::Vector2f endd = sf::Vector2f(end.x, end.y);

    sf::Vertex* points = new sf::Vertex[4]
    {
        sf::Vertex(st + offset, color),
        sf::Vertex(endd + offset, color),
        sf::Vertex(endd - offset, color),
        sf::Vertex(st - offset, color),
    };

    GraphicsDevice::GetWindow()->draw(points, 4, sf::Quads);
}

void Renderer::RenderText_Impl(std::string str, float size, Vector2f pos, sf::Color color)
{
    sf::Text text;
    text.setFont(font);
    text.setPosition(pos.x, pos.y);
    text.setString(str);
    text.setCharacterSize(24);
    text.setFillColor(color);
    GraphicsDevice::GetWindow()->draw(text);
}

void Renderer::Render_Impl(Particle* particle)
{
    Vector2f dir = Vector2f(particle->GetModel()->GetVelocity());
    dir.GetNormalized();

    sf::CircleShape collider;
    float r = particle->GetColliderRadius();
    collider.setPosition(particle->GetModel()->GetPosition().x, particle->GetModel()->GetPosition().y);
    collider.setRadius(r);
    collider.setOrigin(r, r);
    collider.setOutlineThickness(1.0f);

    if (particle->isStatic)
        collider.setOutlineColor(sf::Color::Yellow);
    else
        collider.setOutlineColor(sf::Color::Red);

    collider.setFillColor(sf::Color::Transparent);
    GraphicsDevice::GetWindow()->draw(collider);

    sf::CircleShape search;
    r = PARTICLE_INTERACTION_DISTANCE;
    search.setPosition(particle->GetModel()->GetPosition().x, particle->GetModel()->GetPosition().y);
    search.setRadius(r);
    search.setOrigin(r, r);
    search.setOutlineThickness(1.0f);
    search.setOutlineColor(sf::Color::White);
    search.setFillColor(sf::Color::Transparent);
    //GraphicsDevice::GetWindow()->draw(search);

    RenderLine(particle->GetModel()->GetPosition(), particle->GetModel()->GetPosition() + particle->GetModel()->GetVelocity(), 2.0f, sf::Color::Yellow);
    RenderLine(particle->GetModel()->GetPosition(), particle->GetModel()->GetPosition() + particle->GetModel()->netForce, 5.0f, sf::Color::Green);

    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(2.0f, 2.0f));
    rect.setOrigin(sf::Vector2f(1.0f, 1.0f));
    rect.setFillColor(sf::Color::Red);
    rect.setOutlineColor(sf::Color::Red);
    rect.setPosition(particle->GetModel()->GetPosition().x, particle->GetModel()->GetPosition().y);
    GraphicsDevice::GetWindow()->draw(rect);
}

void Renderer::Render(sf::Shape* shape)
{
    Get()->Render_Impl(shape);
}

void Renderer::Render(Particle* particle)
{
    Get()->Render_Impl(particle);
}

void Renderer::RenderText(std::string str, float size, Vector2f pos, sf::Color color)
{
    Get()->RenderText_Impl(str, size, pos, color);
}

void Renderer::RenderLine(Vector2f start, Vector2f end, float thickness, sf::Color color)
{
    Get()->RenderLine_Impl(start, end, thickness, color);
}

void Renderer::Render(sf::Vector2f position, sf::Vector2f direction, float length)
{
    Get()->Render_Impl(position, direction, length);
}

