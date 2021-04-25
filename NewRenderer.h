#pragma once
#include "Device.h"

class Particle;

class ParticleRenderer
{
private:	
	static ParticleRenderer* instance;

	XMFLOAT4X4	worldMatrix;
	XMFLOAT4X4	viewMatrix;
	XMFLOAT4X4	projectionMatrix;

	SpriteBatch* spriteBatch;

	ParticleRenderer();
	~ParticleRenderer();

	bool isInitialised;

	void Render_Impl(Particle* particle);

public:
	static ParticleRenderer* Get();

	HRESULT Init(HWND window, HINSTANCE instance, int width, int height);
	void	Shutdown();

	void PrepareFrame();
	void PresentFrame();
	static void Render(Particle* particle);
};

