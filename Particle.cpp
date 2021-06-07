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
	Vector2f dist = model.GetPosition() - particle->GetModel()->GetPosition();
	float sumOfRadii = colliderRadius + particle->colliderRadius;
	float length = dist.GetLength();
	Vector2f normalisedDistance = dist.GetNormalized();

	Vector2f pos;
	pos.x = particle->GetModel()->GetPosition().x + ((sumOfRadii + 1) * normalisedDistance.x);
	pos.y = particle->GetModel()->GetPosition().y + ((sumOfRadii + 1) * normalisedDistance.y);
	model.SetPosition(pos);

	//PHYSICS RESPONSE
	Vector2f newVelOne;
	Vector2f newVelTwo;

	//2d elastic collision
	////To degrees
	////float theta = atan(vel.y / vel.x) * (180 / M_PI);
	////theta is the movement angle
	//float thetaOne = atan(GetModel()->GetVelocity().y / GetModel()->GetVelocity().x);
	//float thetaTwo = atan(particle->GetModel()->GetVelocity().y / particle->GetModel()->GetVelocity().x);
	////phi is the contact angle
	//float phiOne = GetModel()->GetVelocity().Dot(particle->GetModel()->GetVelocity());
	//float phiTwo = particle->GetModel()->GetVelocity().Dot(GetModel()->GetVelocity());
	//newVelOne.x = ((GetModel()->GetVelocity().x * cos(thetaOne - phiOne) * (GetModel()->GetMass() - particle->GetModel()->GetMass()) + (2 * particle->GetModel()->GetMass() * particle->GetModel()->GetVelocity().x) * cos(thetaTwo - phiTwo)) / GetModel()->GetMass() - particle->GetModel()->GetMass()) * cos(phiOne) + GetModel()->GetVelocity().x * sin(thetaOne - phiOne) * cos(phiOne + (M_PI / 2));
	//newVelOne.y = ((GetModel()->GetVelocity().y * cos(thetaOne - phiOne) * (GetModel()->GetMass() - particle->GetModel()->GetMass()) + (2 * particle->GetModel()->GetMass() * particle->GetModel()->GetVelocity().y) * cos(thetaTwo - phiTwo)) / GetModel()->GetMass() - particle->GetModel()->GetMass()) * sin(phiOne) + GetModel()->GetVelocity().y * sin(thetaOne - phiOne) * sin(phiOne + (M_PI / 2));
	//newVelTwo.x = ((particle->GetModel()->GetVelocity().x * cos(thetaTwo - phiTwo) * (particle->GetModel()->GetMass() - GetModel()->GetMass()) + (2 * GetModel()->GetMass() * GetModel()->GetVelocity().x) * cos(thetaOne - phiOne)) / particle->GetModel()->GetMass() - GetModel()->GetMass()) * cos(phiTwo) + particle->GetModel()->GetVelocity().x * sin(thetaTwo - phiTwo) * cos(phiTwo + (M_PI / 2));
	//newVelTwo.y = ((particle->GetModel()->GetVelocity().y * cos(thetaTwo - phiTwo) * (particle->GetModel()->GetMass() - GetModel()->GetMass()) + (2 * GetModel()->GetMass() * GetModel()->GetVelocity().y) * cos(thetaOne - phiOne)) / particle->GetModel()->GetMass() - GetModel()->GetMass()) * sin(phiTwo) + particle->GetModel()->GetVelocity().y * sin(thetaTwo - phiTwo) * sin(phiTwo + (M_PI / 2));
	
	//transfer of momentum
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
