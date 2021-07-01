#include "pch.h"
#include "PhysicsModel.h"
#include "Overloads.h"
#include "Particle.h"
#include "SimConsts.h"

#include <iostream>

PhysicsModel::PhysicsModel()
{
	isResting = false;

	position = Vector2f(FLT_MAX, FLT_MAX);
	acceleration = Vector2f(0.0f, 0.0f);
	velocity = Vector2f(0.0f, 0.0f);
	externalForce = Vector2f(0.0f, 0.0f);

	initialDensity = density = DEFAULT_PARTICLE_MASS;
	mass = DEFAULT_PARTICLE_MASS;
	viscosity = 0.0f;
	pressure = 0.0f;

	Reset();
}

PhysicsModel::PhysicsModel(Vector2f position, Vector2f velocity = Vector2f(0.0f, 0.0f))
{
	Reset();
	this->position = position;
	this->velocity = velocity;
	acceleration = Vector2f(0.0f, 0.0f);
	externalForce = Vector2f(0.0f, 0.0f);

	initialDensity = density = DEFAULT_PARTICLE_MASS;
	mass = DEFAULT_PARTICLE_MASS;
	viscosity = 0.0f;
	pressure = 0.0f;
}

Vector2f PhysicsModel::GetPosition()
{
	return position;
}

void PhysicsModel::SetPosition(Vector2f position)
{
	this->position = position;
}

Vector2f PhysicsModel::GetVelocity()
{
	return velocity;
}

void PhysicsModel::SetVelocity(Vector2f velocity)
{
	this->velocity = velocity;
}

float PhysicsModel::GetMass()
{
	return mass;
}

void PhysicsModel::SetMass(float mass)
{
	this->mass = mass;
}

bool PhysicsModel::IsResting()
{
	return isResting;
}

void PhysicsModel::ApplyExternalForce(Vector2f force)
{
	if(force != Vector2f(0.0f, 0.0f) && (!isnan(force.x)))
		externalForce += force;
}

void PhysicsModel::Update(float DeltaTime)
{
	netForce = Vector2f();

	CalculateParticleDensityAndPressure();
	UpdateSPH();

	previousPosition = position;

	acceleration = (netForce / density);
	velocity = velocity + (DeltaTime * acceleration = (netForce / density));
	position = position + (velocity * DeltaTime);

	EnforceEdges();
}

void PhysicsModel::EnforceEdges()
{
	// enforce boundary conditions
	if (position.x - WORLD_EDGE < 0.0f)
	{
		velocity.x *= DAMPENING;
		position.x = WORLD_EDGE;
	}

	if (position.x + WORLD_EDGE > WORLD_SIZE.x)
	{
		velocity.x *= DAMPENING;
		position.x = WORLD_SIZE.x - WORLD_EDGE;
	}

	if (position.y - WORLD_EDGE < 0.0f)
	{
		velocity.y *= DAMPENING;
		position.y = WORLD_EDGE;
	}
	if (position.y + WORLD_EDGE > WORLD_SIZE.y)
	{
		velocity.y *= DAMPENING;
		position.y = WORLD_SIZE.y - WORLD_EDGE;
	}
}

void PhysicsModel::Reset()
{
	position = Vector2f(FLT_MAX, FLT_MAX);
	acceleration = Vector2f(0.0f, 0.0f);
	velocity = Vector2f(0.0f, 0.0f);
	externalForce = Vector2f(0.0f, 0.0f);

	initialDensity = density = DEFAULT_PARTICLE_MASS;
	viscosity = 10.0f;
	mass = DEFAULT_PARTICLE_MASS;
	pressure = 5.0f;
}

void PhysicsModel::CalculateParticleDensityAndPressure()
{
	//could make equal to 0;
	density = initialDensity;
	density += mass * Poly6(0.0f);

	for (auto p : LocalParticles)
	{
		Vector2f diff = position - p->GetModel()->GetPosition();
		float dist = diff.GetLength();

		density += (p->GetModel()->GetMass() * Poly6(dist * dist));

	}
	//could remove max;
	//pressure = std::max(GAS_CONSTANT * (density - REST_DENSITY), 0.0f);
	pressure = GAS_CONSTANT * (density - REST_DENSITY);

	/*std::string s = "Pressure: " + std::to_string(pressure) + '\n';
	s += "Density : " + std::to_string(density) + '\n';
	OutputDebugStringA(s.c_str());*/
}

void PhysicsModel::UpdateSPH()
{
	Vector2f fPressure;
	Vector2f sumPressure;
	Vector2f sumViscosity;
	Vector2f fViscosity;
	Vector2f fSurfaceTension;

	for (auto p : LocalParticles)
	{
		Vector2f diff = position - p->GetModel()->GetPosition();
		float dist = diff.GetLength();

		if (dist != 0.0f)
		{
			sumPressure += diff.GetNormalized() * (p->GetModel()->mass / p->GetModel()->density) *
				((pressure + p->GetModel()->pressure) / 2)
				* Spiky(dist);

			sumViscosity += diff.GetNormalized() * (p->GetModel()->mass / p->GetModel()->density) *
				(p->GetModel()->GetVelocity() - velocity) * ViscoKernel(dist);
		}
	}

	if (LocalParticles.size() != 0)
	{
		fPressure = sumPressure;
		fViscosity = VISCOSITY_CONSTANT * sumViscosity;

		netForce += fPressure;
		netForce += fViscosity;
		netForce += fSurfaceTension;
	}

	netForce += density * GRAVITY;
}

//Poly6 Kernel for Density calculations
float PhysicsModel::Poly6(float radius_square)
{
	if (radius_square >= 0.0f && radius_square <= KERNEL_HEIGHT)
		return 315.0f / (64.0f * M_PI * pow(KERNEL_HEIGHT, 9)) * pow(KERNEL_HEIGHT * KERNEL_HEIGHT - radius_square, 3);
	else
		return 0.0f;
}

//Spiky kernel for Pressure calculations
float PhysicsModel::Spiky(float radius)
{
	if (radius >= 0.0f && radius <= KERNEL_HEIGHT)
		return -45.0f / (M_PI * pow(KERNEL_HEIGHT, 6)) * pow(KERNEL_HEIGHT - radius, 3);
	else
		return 0.0f;
}

//Kernel for viscosity calculations
float PhysicsModel::ViscoKernel(float radius)
{
	if (radius >= 0.0f && radius <= KERNEL_HEIGHT)
		return -15.0f / (2 * M_PI * pow(KERNEL_HEIGHT, 3)) * (0.0f - ((radius * radius * radius) / (2 * (KERNEL_HEIGHT * KERNEL_HEIGHT * KERNEL_HEIGHT))) + ((radius * radius) / (KERNEL_HEIGHT * KERNEL_HEIGHT)) + (KERNEL_HEIGHT / (2 * radius)) - 1);
	else
		return 0.0f;
}