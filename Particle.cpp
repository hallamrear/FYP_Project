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
	Vector2f pos;
	pos.x = (float)(rand() % (int)WORLD_SIZE.x + 1);
	pos.y = (float)(rand() % (int)WORLD_SIZE.y + 1);
	model.SetPosition(pos);

	Vector2f vel;
	vel.x = (float)(rand() % (int)(WORLD_SIZE.x) + 1) - (WORLD_SIZE.x / 2.0f);
	vel.y = (float)(rand() % (int)(WORLD_SIZE.y) + 1) - (WORLD_SIZE.y / 2.0f);
	model.SetVelocity(vel);

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
	Vector2f distance = model.GetPosition() - particle->GetModel()->GetPosition();
	float sumOfRadii = colliderRadius + particle->colliderRadius;
	float length = distance.GetLength();
	Vector2f normalisedDistance = distance.GetNormalized();

	Vector2f pos;
	pos.x = particle->GetModel()->GetPosition().x + ((sumOfRadii + 1) * normalisedDistance.x);
	pos.y = particle->GetModel()->GetPosition().y + ((sumOfRadii + 1) * normalisedDistance.y);
	model.SetPosition(pos);

	//PHYSICS RESPONSE
	//2d elastic collision
	Vector2f newVelOne;
	Vector2f newVelTwo;

	newVelOne.x = (GetModel()->GetVelocity().x * (GetModel()->GetMass() - particle->GetModel()->GetMass()) + (2 * particle->GetModel()->GetMass() * particle->GetModel()->GetVelocity().x)) / (GetModel()->GetMass() + particle->GetModel()->GetMass());
	newVelOne.y = (GetModel()->GetVelocity().y * (GetModel()->GetMass() - particle->GetModel()->GetMass()) + (2 * particle->GetModel()->GetMass() * particle->GetModel()->GetVelocity().y)) / (GetModel()->GetMass() + particle->GetModel()->GetMass());
	newVelTwo.x = (particle->GetModel()->GetVelocity().x * (particle->GetModel()->GetMass() - GetModel()->GetMass()) + (2 * GetModel()->GetMass() * GetModel()->GetVelocity().x)) / (GetModel()->GetMass() + particle->GetModel()->GetMass());
	newVelTwo.y = (particle->GetModel()->GetVelocity().y * (particle->GetModel()->GetMass() - GetModel()->GetMass()) + (2 * GetModel()->GetMass() * GetModel()->GetVelocity().y)) / (GetModel()->GetMass() + particle->GetModel()->GetMass());

	GetModel()->SetVelocity(newVelOne * DAMPENING);
	particle->GetModel()->SetVelocity(newVelTwo * DAMPENING);

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
