#pragma once
#include "SimConsts.h"
#include "Device.h"
#include "Vector.h"

struct alignas(16) ConstantBuffer
{
	Vector2f MousePosition;
	float DeltaTime;
	float ElapsedTime;
	Vector2f Particles[MAX_PARTICLE_COUNT];
};

class Particle;
class TTF_Font;
class Shape;

class Renderer
{
private:
	static Renderer* instance;

	SDL_GPUTexture* m_GPUTexture;
	SDL_GPUShader* m_ComputeShader;
	SDL_GPUComputePass* m_CurrentComputePass;
	SDL_GPUCommandBuffer* m_CommandBuffer;
	SDL_GPUComputePipeline* m_ComputePipeline;
	char* m_ShaderByteCode;
	size_t m_ShaderByteCodeSize;
	SDL_GPUBuffer* m_GPUStorageBuffer;
	SDL_GPUTransferBuffer* m_DataTransferBuffer;
	SDL_GPUTexture* m_SwapchainTexture;
	SDL_GPUDevice* m_GPUDevice;
	SDL_Renderer* m_Renderer;
	SDL_Texture* m_ParticleImage;
	SDL_GPUTransferBuffer* m_ConstantBufferTransferBuffer;
	SDL_GPUBuffer* m_GPUConstantBuffer;
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
	ConstantBuffer ConstantBuffer;
	Renderer(const Renderer&) = delete;

	float ClearColour[4] = {};

	HRESULT Init(int width, int height);
	static Renderer* Get();

	void ComputePass();

	void PrepareFrame();
	void PresentFrame();

	static void RenderText(std::string str, float size, Vector2f pos, SDL_Color color);
	static void RenderLine(Vector2f start, Vector2f end, float thickness, SDL_Color color);
	static void RenderVector(Vector2f position, Vector2f direction, float length);
	static void RenderParticle(Particle* particle);
	static void RenderParticleDetailed(Particle* particle);
	static void RenderShape(Shape* shape);
};

