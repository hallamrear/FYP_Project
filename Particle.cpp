#include "pch.h"
#include "Particle.h"
#include <random>
#include <chrono>
#include "Overloads.h"
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
	circleCollider.r = 10.0f;
	colliderRadius = circleCollider.r;

	Kill();
}

Particle::~Particle()
{
	Kill();
}

void Particle::Create()
{
	model = PhysicsModel();

	circleCollider.p.x = model.position.x;
	circleCollider.p.y = model.position.y;

	isAlive = true;
}

void Particle::CreateRandom()
{
	XMFLOAT2 position = XMFLOAT2((float)(rand() % (int)WORLD_SIZE.x + 1), (float)(rand() % (int)WORLD_SIZE.y + 1));
	model.position = position;

	XMFLOAT2 vel = XMFLOAT2((float)(rand() % (int)(WORLD_SIZE.x / 2) + 1), (float)(rand() % (int)(WORLD_SIZE.y / 2) + 1));
	vel.y = 0.0f;
	model.velocity = vel;

	circleCollider.p.x = model.position.x;
	circleCollider.p.y = model.position.y;

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
	if (this == particle)
		return;

	PhysicsModel* model = GetModel();

	//Normal of collision
	XMFLOAT2 collisionNormal = GetModel()->position - particle->GetModel()->position;
	XMStoreFloat2(&collisionNormal, XMVector2Normalize(XMLoadFloat2(&collisionNormal)));

	//The direction of the collision plane, perpendicular to the normal
	XMFLOAT2 collisionDirection = XMFLOAT2(-collisionNormal.y, collisionNormal.x);

	XMFLOAT2 v1Parallel;
	XMStoreFloat2(&v1Parallel, XMVector2Dot(XMLoadFloat2(&collisionNormal), XMLoadFloat2(&GetModel()->velocity)));
	v1Parallel = v1Parallel * collisionNormal;

	XMFLOAT2 v1Ortho;
	XMStoreFloat2(&v1Ortho, XMVector2Dot(XMLoadFloat2(&collisionDirection), XMLoadFloat2(&GetModel()->velocity)));
	v1Ortho = v1Ortho * collisionDirection;


	XMFLOAT2 v2Parallel;
	XMStoreFloat2(&v2Parallel, XMVector2Dot(XMLoadFloat2(&collisionNormal), XMLoadFloat2(&particle->GetModel()->velocity)));
	v2Parallel = v2Parallel * collisionNormal;

	XMFLOAT2 v2Ortho;
	XMStoreFloat2(&v2Ortho, XMVector2Dot(XMLoadFloat2(&collisionDirection), XMLoadFloat2(&particle->GetModel()->velocity)));
	v2Ortho = v2Ortho * collisionDirection;


	XMFLOAT2 v1 = v1Parallel + v1Ortho;
	XMFLOAT2 v2 = v2Parallel + v2Ortho;




	XMFLOAT2 v1Length;
	XMStoreFloat2(&v1Length, XMVector2Length(XMLoadFloat2(&v1Parallel)));
	XMFLOAT2 v2Length;
	XMStoreFloat2(&v2Length, XMVector2Length(XMLoadFloat2(&v2Parallel)));

	XMFLOAT2 commonVelocity = 2 * (GetModel()->mass * v1Length + particle->GetModel()->mass * v2Length) / (GetModel()->mass + particle->GetModel()->mass);
	XMFLOAT2 v1LengthAfterCollision = commonVelocity - v1Length;
	XMFLOAT2 v2LengthAfterCollision = commonVelocity - v2Length;
	v1Parallel = v1Parallel * (v1LengthAfterCollision / v1Length);
	v2Parallel = v2Parallel * (v2LengthAfterCollision / v2Length);


	float totalMass = GetModel()->mass + particle->GetModel()->mass;
	XMFLOAT2 v1ParallelNew = (v1Parallel * (GetModel()->mass - particle->GetModel()->mass) + 2 * particle->GetModel()->mass * v2Parallel) / totalMass;
	XMFLOAT2 v2ParallelNew = (v2Parallel * (particle->GetModel()->mass - GetModel()->mass) + 2 * GetModel()->mass * v1Parallel) / totalMass;
	v1Parallel = v1ParallelNew;
	v2Parallel = v2ParallelNew;

	GetModel()->velocity = v1Parallel + v1Ortho;
	particle->GetModel()->velocity = v2Parallel + v2Ortho;









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
