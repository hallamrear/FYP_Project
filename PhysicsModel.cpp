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

void PhysicsModel::UpdateSPH(float DeltaTime)
{
	velocity += GRAVITY / density;

	//Viscosity
	CalculateViscosity(DeltaTime);

	//Update position
	previousPosition = position;
	position += velocity * DeltaTime;

	//Density
	CalculateDoubleDensity(DeltaTime);

	//Make new velocity
	velocity = (position - previousPosition) / DeltaTime;

	position += velocity * DeltaTime;

	//Enforce bounds
	EnforceEdges();
}


void PhysicsModel::CalculateDoubleDensity(float DeltaTime)
{
	float density = 0;
	float neardensity = 0;
	for (auto& j : LocalParticles)
	{
		if (j->isStatic == false)
		{
			Vector2f diff = j->GetModel()->position - position;
			float dist = diff.GetLength();
			float q = dist / PARTICLE_INTERACTION_DISTANCE;
			if (q < 1 && q != 0)
			{
				density += (1.0f - q) * (1.0f - q);
				neardensity += (1.0f - q) * (1.0f - q) * (1.0f - q);
			}
		}
	}

	float knear = NEAR_STIFFNESS_PARAM;

	float P = GAS_CONSTANT * (density - REST_DENSITY);
	float Pnear = knear * neardensity;
	Vector2f dx = Vector2f();
	for (auto& j : LocalParticles)
	{
		if (j->isStatic == false)
		{
			Vector2f rij = j->GetModel()->position - position;
			float rijmag = rij.GetLength();
			float q = rijmag / PARTICLE_INTERACTION_DISTANCE;
			if (q < 1 && q != 0)
			{
				Vector2f rij_n = rij.GetNormalized();
				//Incompressibility relaxation
				Vector2f D = rij_n * (DeltaTime * DeltaTime * (P * (1.0f - q)) + Pnear * (1.0f - q) * (1.0f - q));
				j->GetModel()->position += (D / 2);
				dx -= (D / 2);
			}
		}
	}
	position += dx;

	this->density = density;
	this->nearDensity = neardensity;
}

void PhysicsModel::CalculatePressure()
{

}

void PhysicsModel::CalculateViscosity(float DeltaTime)
{
	for (auto& j : LocalParticles)
	{
		if (j->isStatic == false)
		{
			Vector2f diff = (j->GetModel()->position - position);
			float q = diff.GetLength() / PARTICLE_INTERACTION_DISTANCE;

			if (q < 1 && q != 0)
			{
				Vector2f diff_n = diff.GetNormalized();
				float u = (velocity - j->GetModel()->velocity).Dot(diff_n);
				if (u > 0)
				{
					//TODO : dunno if this is legit
					Vector2f impulse = diff_n * ((1 - q) * (VISCOSITY_CONSTANT_SIGMA * u + VISCOSITY_CONSTANT_BETA * u * u)) * DeltaTime;
					velocity += (-impulse / 2.0f);
					j->GetModel()->velocity += (impulse / 2.0f);
				}
			}
		}
	}
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
