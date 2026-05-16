#include "PCH.h"
#include "Renderer.h"
#include "Particle.h"
#include "Overloads.h"

Renderer* Renderer::instance;

Renderer::Renderer()
{
    m_ParticleImage = nullptr;

    m_Renderer = nullptr;
    WindowHeight = 0;
    WindowWidth = 0;
    isInitialised = false;
    m_ParticleRectListBack = 0;
    m_ParticleRects = std::vector<SDL_FRect>();
    m_ParticleRects.resize(MAX_PARTICLE_COUNT);
    memset(&m_ParticleRects[0], 0, sizeof(SDL_FRect) * MAX_PARTICLE_COUNT);
    font = nullptr;
}

Renderer::~Renderer()
{
    
}

HRESULT Renderer::Init(int width, int height)
{
    HRESULT hr = E_FAIL;

    WindowWidth = width;
    WindowHeight = height;

    if (GraphicsDevice::Get()->GetIsInitialised() == false)
    {
        GraphicsDevice::Get()->Init(width, height);
    }

    m_Renderer = SDL_CreateRenderer(GraphicsDevice::GetWindow(), nullptr);

    m_ParticleImage = IMG_Load("Particle.png");

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
    SDL_SetRenderDrawColorFloat(m_Renderer, 0.0f, 0.0f, 0.0f, 1.0f);
    SDL_RenderClear(m_Renderer);
}

void Renderer::PresentFrame()
{
    SDL_SetRenderDrawColorFloat(m_Renderer, 1.0F, 0.0f, 0.0f, 1.0f);
    SDL_RenderRects(m_Renderer, &m_ParticleRects[0], m_ParticleRectListBack);
    SDL_RenderPresent(m_Renderer);

    memset(&m_ParticleRects[0], 0, sizeof(SDL_FRect) * MAX_PARTICLE_COUNT);
    m_ParticleRectListBack = 0;
}

void Renderer::RenderShape_Impl(Shape* shape)
{
    //GraphicsDevice::GetWindow()->draw(*shape);
}

void Renderer::RenderVector_Impl(Vector2f position, Vector2f direction, float length)
{
    Vector2f end = { };
    end.x = position.x + (direction.x * length);
    end.y = position.y + (direction.y * length);
    SDL_SetRenderDrawColorFloat(m_Renderer, 1.0f, 0.0f, 1.0f, 1.0f);
    SDL_RenderLine(m_Renderer, position.x, position.y, end.x, end.y);
}

void Renderer::RenderLine_Impl(Vector2f start, Vector2f end, float thickness, SDL_Color color)
{
    SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, 255);
    SDL_RenderLine(m_Renderer, start.x, start.y, end.x, end.y);
}

void Renderer::RenderText_Impl(std::string str, float size, Vector2f pos, SDL_Color color)
{
    SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, 255);
   /* sf::Text text;
    text.setFont(font);
    text.setPosition(pos.x, pos.y);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    GraphicsDevice::GetWindow()->draw(text);*/
    OutputDebugString(L"RenderText_Impl not implemented.\n");
}

void Renderer::RenderParticle_Impl(Particle* particle)
{
    //TODO : Draw Circles instead.

    float r = 3.0f;

    SDL_FRect rect{};
    m_ParticleRects[m_ParticleRectListBack].x = particle->GetModel()->GetPosition().x - r;
    m_ParticleRects[m_ParticleRectListBack].y = particle->GetModel()->GetPosition().y - r;
    m_ParticleRects[m_ParticleRectListBack].w = r + r;
    m_ParticleRects[m_ParticleRectListBack].h = r + r;

    if (particle->isStatic)
        SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
    else
        SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 255);

    m_ParticleRectListBack++;

    //SDL_RenderRect(m_Renderer, &rect);
}

void Renderer::RenderParticleDetailed_Impl(Particle* particle)
{
    RenderParticle_Impl(particle);

    Vector2f dir = Vector2f(particle->GetModel()->GetVelocity());
    dir.GetNormalized();
    RenderLine(particle->GetModel()->GetPosition(), particle->GetModel()->GetPosition() + dir, 2.0f, {255, 255, 0, 255});

    SDL_Rect rect{};
    rect.x = particle->GetModel()->GetPosition().x - 1.0f;
    rect.y = particle->GetModel()->GetPosition().y - 1.0f;
    rect.w = 2.0f;
    rect.h = 2.0f;
    SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 255);
}

void Renderer::RenderShape(Shape* shape)
{
    Get()->RenderShape_Impl(shape);
}

void Renderer::RenderText(std::string str, float size, Vector2f pos, SDL_Color color)
{
    Get()->RenderText_Impl(str, size, pos, color);
}

void Renderer::RenderLine(Vector2f start, Vector2f end, float thickness, SDL_Color color)
{
    Get()->RenderLine_Impl(start, end, thickness, color);
}

void Renderer::RenderVector(Vector2f position, Vector2f direction, float length)
{
    Get()->RenderVector_Impl(position, direction, length);
}

void Renderer::RenderParticle(Particle* particle)
{
    Get()->RenderParticle_Impl(particle);
}

void Renderer::RenderParticleDetailed(Particle* particle)
{
    Get()->RenderParticleDetailed_Impl(particle);
}

