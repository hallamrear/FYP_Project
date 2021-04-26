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

	circleCollider = c2Circle();
	circleCollider.r = 2.0f;

	Kill();
}

Particle::~Particle()
{
	Kill();
}

void Particle::Create()
{
	model = PhysicsModel();

	isAlive = true;
}

void Particle::CreateRandom()
{
	XMFLOAT2 position = XMFLOAT2((float)(rand() % (int)WORLD_SIZE.x + 1), (float)(rand() % (int)WORLD_SIZE.y + 1));
	model.position = position;

	XMFLOAT2 vel = XMFLOAT2((float)(rand() % (int)(WORLD_SIZE.x / 2) + 1), (float)(rand() % (int)(WORLD_SIZE.y / 2) + 1));
	vel.y = 0.0f;
	model.velocity = vel;

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

PhysicsModel* Particle::GetModel()
{
	return &model;
}

c2Circle Particle::GetCollider()
{
	return circleCollider;
}

void Particle::ResolveCollision(Particle* particle)
{

}

void Particle::Update(float DeltaTime)
{
	if (!isAlive)
		return;

	model.Update(DeltaTime);

	circleCollider.p.x = model.position.x;
	circleCollider.p.y = model.position.y;
}

ID3D11ShaderResourceView* Particle::GetTexture()
{
	return texture;
}
