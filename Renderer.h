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
	ID3D11SamplerState* linearSampler;
	ID3D11BlendState* NoBlendState;
	ID3D11RasterizerState* NoCullSolidFillState;
	SpriteBatch* spriteBatch;

	ID3D11Texture2D* rtTex;
	ID3D11RenderTargetView* rtView;
	ID3D11ShaderResourceView* rtSRV;

	ID3D11ShaderResourceView* outlineTexture;
	ID3D11Resource* outlineResource;

	XMFLOAT2 ViewPosition;
	bool IsViewMatrixDirty;

	bool isInitialised;

	Renderer();
	~Renderer();

	void Render_Impl(Particle* particle);
public:
	Renderer(const Renderer&) = delete;

	float ClearColour[4] = {};
	ConstantBuffer constantBuffer;

	HRESULT Init(HINSTANCE hInstance, HWND hWindow, int width, int height);
	static Renderer* Get();

	void AlterViewPosition(XMFLOAT2 adjustment);

	void PrepareFrame();
	void PresentFrame();

	void PrepareGeometryRender();

	static void Render(Particle* particle);
};

