#pragma once
#include "Device.h"
#include "Vector.h"

class Particle;
class TTF_Font;
class Shape;
class Renderer
{
private:
	static Renderer* instance;

	SDL_Renderer* m_Renderer;
	TTF_Font* font;

	int WindowWidth;
	int WindowHeight;

	bool isInitialised;
	std::vector<SDL_FRect> m_ParticleRects;
	int m_ParticleRectListBack;

	Renderer();
	~Renderer();

	void RenderParticle_Impl(Particle* particle);
	void RenderParticleDetailed_Impl(Particle* particle);
	void RenderShape_Impl(Shape* shape);
	void RenderVector_Impl(Vector2f position, Vector2f direction, float length);
	void RenderLine_Impl(Vector2f start, Vector2f end, float thickness, SDL_Color color);
	void RenderText_Impl(std::string str, float size, Vector2f pos, SDL_Color color);

public:
	Renderer(const Renderer&) = delete;

	float ClearColour[4] = {};

	HRESULT Init(int width, int height);
	static Renderer* Get();

	void PrepareFrame();
	void PresentFrame();

	static void RenderText(std::string str, float size, Vector2f pos, SDL_Color color);
	static void RenderLine(Vector2f start, Vector2f end, float thickness, SDL_Color color);
	static void RenderVector(Vector2f position, Vector2f direction, float length);
	static void RenderParticle(Particle* particle);
	static void RenderParticleDetailed(Particle* particle);
	static void RenderShape(Shape* shape);
};

