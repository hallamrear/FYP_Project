#include "pch.h"
#include "Particle.h"
#include <random>
#include <chrono>
#include "Overloads.h"
#include "Device.h"

Particle::Particle()
{
	srand(time(0));
	colliderRadius = PARTICLE_COLLIDER_SIZE;
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
	model.position.x = (float)(rand() % (int)WORLD_SIZE.x + 1);
	model.position.y = (float)(rand() % (int)WORLD_SIZE.y + 1);

	model.velocity.x = (float)(rand() % (int)(WORLD_SIZE.x) + 1) - (WORLD_SIZE.x / 2.0f);
	model.velocity.y = (float)(rand() % (int)(WORLD_SIZE.y) + 1) - (WORLD_SIZE.y / 2.0f);
	
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

void Particle::ResolveCollision(Particle* particle)
{
	if (this == particle)
		return;

	////COLLISION RESPONSE
	//// normalise the vector between them
	Vector2f distance = model.position - particle->GetModel()->position;
	float sumOfRadii = colliderRadius + particle->colliderRadius;
	float length = distance.GetLength();
	Vector2f normalisedDistance = distance.GetNormalized();

	model.position.x = particle->GetModel()->position.x + ((sumOfRadii + 1) * normalisedDistance.x);
	model.position.y = particle->GetModel()->position.y + ((sumOfRadii + 1) * normalisedDistance.y);

	//PHYSICS RESPONSE
	//2d elastic collision
	Vector2f newVelOne;
	Vector2f newVelTwo;

	newVelOne.x = (GetModel()->velocity.x * (GetModel()->mass - particle->GetModel()->mass) + (2 * particle->GetModel()->mass * particle->GetModel()->velocity.x)) / (GetModel()->mass + particle->GetModel()->mass);
	newVelOne.y = (GetModel()->velocity.y * (GetModel()->mass - particle->GetModel()->mass) + (2 * particle->GetModel()->mass * particle->GetModel()->velocity.y)) / (GetModel()->mass + particle->GetModel()->mass);
	newVelTwo.x = (particle->GetModel()->velocity.x * (particle->GetModel()->mass - GetModel()->mass) + (2 * GetModel()->mass * GetModel()->velocity.x)) / (GetModel()->mass + particle->GetModel()->mass);
	newVelTwo.y = (particle->GetModel()->velocity.y * (particle->GetModel()->mass - GetModel()->mass) + (2 * GetModel()->mass * GetModel()->velocity.y)) / (GetModel()->mass + particle->GetModel()->mass);

	GetModel()->velocity = newVelOne * 0.95f;
	particle->GetModel()->velocity = newVelTwo * 0.95f;

}

float Particle::GetColliderRadius()
{
	return colliderRadius;
}

void Particle::Update(float DeltaTime)
{
	if (!isAlive)
		return;

	model.Update(DeltaTime);
}
