#pragma once
#include "Device.h"
#include "Vector.h"

#define VS_SHADER "VertexShader.hlsl"
#define PS_SHADER "PixelShader.hlsl"

class Particle;

class Renderer
{
private:
	static Renderer* instance;

	sf::Font font;

	int WindowWidth;
	int WindowHeight;

	bool isInitialised;

	Renderer();
	~Renderer();

	void Render_Impl(Particle* particle);
	void Render_Impl(sf::Shape* shape);
	void Render_Impl(sf::Vector2f position, sf::Vector2f direction, float length);
	void RenderLine_Impl(Vector2f start, Vector2f end, float thickness, sf::Color color);
	void RenderText_Impl(std::string str, float size, Vector2f pos, sf::Color color);

public:
	Renderer(const Renderer&) = delete;

	float ClearColour[4] = {};

	HRESULT Init(int width, int height);
	static Renderer* Get();

	void PrepareFrame();
	void PresentFrame();

	static void RenderText(std::string str, float size, Vector2f pos, sf::Color color);
	static void RenderLine(Vector2f start, Vector2f end, float thickness, sf::Color color);
	static void Render(sf::Vector2f position, sf::Vector2f direction, float length);
	static void Render(Particle* particle);
	static void Render(sf::Shape* particle);
};

