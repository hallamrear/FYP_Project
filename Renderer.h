#pragma once
#include "Device.h"

#define VS_SHADER "VertexShader.hlsl"
#define PS_SHADER "PixelShader.hlsl"

class Particle;

class Renderer
{
private:
	static Renderer* instance;

	int WindowWidth;
	int WindowHeight;

	bool isInitialised;

	Renderer();
	~Renderer();

	void Render_Impl(Particle* particle);
	void Render_Impl(sf::Shape* shape);

public:
	Renderer(const Renderer&) = delete;

	float ClearColour[4] = {};

	HRESULT Init(int width, int height);
	static Renderer* Get();

	void PrepareFrame();
	void PresentFrame();



	static void Render(Particle* particle);
	static void Render(sf::Shape* particle);
};

