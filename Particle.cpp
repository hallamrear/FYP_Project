#include "pch.h"
#include "Particle.h"
#include <random>
#include <chrono>
#include "Overloads.h"
#include "Device.h"
#include "SimConsts.h"

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
	isStatic = false;
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
	isStatic = false;
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

	Vector2f dist = model.GetPosition() - particle->GetModel()->GetPosition();
	Vector2f vel = model.GetVelocity();
	model.SetVelocity(dist + vel);

	//////COLLISION RESPONSE
	////// normalise the vector between them

	float sumOfRadii = colliderRadius + particle->colliderRadius;
	float length = dist.GetLength();
	Vector2f normalisedDistance = dist.GetNormalized();

	Vector2f pos;
	pos.x = particle->GetModel()->GetPosition().x + ((sumOfRadii + 1) * normalisedDistance.x);
	pos.y = particle->GetModel()->GetPosition().y + ((sumOfRadii + 1) * normalisedDistance.y);
	//model.SetPosition(pos);

	////PHYSICS RESPONSE

	//2d collision
	float thisDirectionAngle = atan(GetModel()->GetVelocity().y / GetModel()->GetVelocity().x) * (180 / M_PI);
	float otherDirectionAngle = atan(particle->GetModel()->GetVelocity().y / particle->GetModel()->GetVelocity().x) * (180 / M_PI);
	float thisContactAngle = GetModel()->GetVelocity().Dot(particle->GetModel()->GetVelocity());
	float otherContactAngle = particle->GetModel()->GetVelocity().Dot(GetModel()->GetVelocity());

	Vector2f newVelOne;
	Vector2f newVelTwo;
	newVelOne.x = ((GetModel()->GetVelocity().x * cos(thisDirectionAngle - thisContactAngle) * (GetModel()->GetMass() - particle->GetModel()->GetMass()) + (2 * particle->GetModel()->GetMass() * particle->GetModel()->GetVelocity().x) * cos(otherDirectionAngle - otherContactAngle)) / GetModel()->GetMass() - particle->GetModel()->GetMass()) * cos(thisContactAngle) + GetModel()->GetVelocity().x * sin(thisDirectionAngle - thisContactAngle) * cos(thisContactAngle + (M_PI / 2));
	newVelOne.y = ((GetModel()->GetVelocity().y * cos(thisDirectionAngle - thisContactAngle) * (GetModel()->GetMass() - particle->GetModel()->GetMass()) + (2 * particle->GetModel()->GetMass() * particle->GetModel()->GetVelocity().y) * cos(otherDirectionAngle - otherContactAngle)) / GetModel()->GetMass() - particle->GetModel()->GetMass()) * sin(thisContactAngle) + GetModel()->GetVelocity().y * sin(thisDirectionAngle - thisContactAngle) * sin(thisContactAngle + (M_PI / 2));
	newVelTwo.x = ((particle->GetModel()->GetVelocity().x * cos(otherDirectionAngle - otherContactAngle) * (particle->GetModel()->GetMass() - GetModel()->GetMass()) + (2 * GetModel()->GetMass() * GetModel()->GetVelocity().x) * cos(thisDirectionAngle - thisContactAngle)) / particle->GetModel()->GetMass() - GetModel()->GetMass()) * cos(otherContactAngle) + particle->GetModel()->GetVelocity().x * sin(otherDirectionAngle - otherContactAngle) * cos(otherContactAngle + (M_PI / 2));
	newVelTwo.y = ((particle->GetModel()->GetVelocity().y * cos(otherDirectionAngle - otherContactAngle) * (particle->GetModel()->GetMass() - GetModel()->GetMass()) + (2 * GetModel()->GetMass() * GetModel()->GetVelocity().y) * cos(thisDirectionAngle - thisContactAngle)) / particle->GetModel()->GetMass() - GetModel()->GetMass()) * sin(otherContactAngle) + particle->GetModel()->GetVelocity().y * sin(otherDirectionAngle - otherContactAngle) * sin(otherContactAngle + (M_PI / 2));

	//GetModel()->SetVelocity(newVelOne * abs(DAMPENING));
	//particle->GetModel()->SetVelocity(newVelTwo * abs(DAMPENING));
}

float Particle::GetColliderRadius()
{
	return colliderRadius;
}

void Particle::Update(float DeltaTime)
{
	if (!isAlive)
		return;

	if(isStatic == false)
		model.UpdateSPH(DeltaTime);
}
