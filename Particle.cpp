#include "pch.h"
#include "Particle.h"
#include <random>
#include <chrono>
#include "Device.h"

ID3D11Resource* Particle::textureResource;
ID3D11ShaderResourceView* Particle::texture;

Particle::Particle()
{
	srand(time(0));

	if (texture == nullptr && textureResource == nullptr)
	{
		CreateDDSTextureFromFile(
			GraphicsDevice::GetDevice(),
			L"Resources/Particle.dds",
			&textureResource,
			&texture);
	}

	Kill();
}

Particle::~Particle()
{
	Kill();
}

void Particle::Create()
{
	model = PhysicsModel();

	model.Update(0.0001f);

	isAlive = true;
}

void Particle::CreateRandom()
{
	XMFLOAT2 position = XMFLOAT2((float)(rand() % (int)WORLD_SIZE.x + 1), (float)(rand() % (int)WORLD_SIZE.y + 1));
	model.position = position;

	isAlive = true;
}

void Particle::Kill()
{
	isAlive = false;
	model.Reset();
}

void Particle::SetAlive(bool state)
{
	isAlive = state;
}

bool Particle::GetAlive()
{
	return isAlive;
}

void Particle::Update(float DeltaTime)
{
	if (!isAlive)
		return;

	model.Update(DeltaTime);
}

ID3D11ShaderResourceView* Particle::GetTexture()
{
	return texture;
}
