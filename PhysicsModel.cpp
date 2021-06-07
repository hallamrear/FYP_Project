#include "pch.h"
#include "PhysicsModel.h"
#include "Overloads.h"
#include "Particle.h"

PhysicsModel::PhysicsModel()
{
	isResting = false;

	position = Vector2f(FLT_MAX, FLT_MAX);
	acceleration = Vector2f(0.0f, 0.0f);
	velocity = Vector2f(0.0f, 0.0f);
	externalForce = Vector2f(0.0f, 0.0f);

	initialDensity = density = DEFAULT_PARTICLE_MASS;
	viscosity = 10.0f;
	mass = DEFAULT_PARTICLE_MASS;

	pressure = 5.0f;

	Reset();
}

PhysicsModel::PhysicsModel(Vector2f position, Vector2f velocity = Vector2f(0.0f, 0.0f))
{
	Reset();
	this->position = position;
	this->velocity = velocity;
	acceleration = Vector2f(0.0f, 0.0f);
	externalForce = Vector2f(0.0f, 0.0f);

	density = DEFAULT_PARTICLE_MASS;
	viscosity = 10.0f;
	mass = DEFAULT_PARTICLE_MASS;
	pressure = 5.0f;
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
	if (position.x < WORLD_EDGE || position.x > WORLD_SIZE.x - WORLD_EDGE)
	{
		if (position.x < WORLD_EDGE)
			position.x = WORLD_EDGE;
		else
			position.x = WORLD_SIZE.x - WORLD_EDGE;

		velocity.x *= -DAMPENING;
	}

	if (position.y < WORLD_EDGE || position.y > WORLD_SIZE.y - WORLD_EDGE)
	{
		if (position.y < WORLD_EDGE)
			position.y = WORLD_EDGE;
		else
			position.y = WORLD_SIZE.y - WORLD_EDGE;

		velocity.y *= -DAMPENING;
	}

	UpdateSPH();

	velocity += netForce / mass;

	position.x += ((velocity.x * DeltaTime) * DAMPENING);
	position.y += ((velocity.y * DeltaTime) * DAMPENING);

	externalForce = Vector2f(0.0f, 0.0f);
	netForce = Vector2f(0.0f, 0.0f);

	previousPosition = position;
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
	//Calculate density and pressure from number of particles surrounding it

	int size = LocalParticles.size();
	for (int particleCount = 0; particleCount < size; particleCount++)
	{
		Vector2f dist = LocalParticles[particleCount]->GetModel()->position - position;
		float distSquared = dist.GetLength() * dist.GetLength();

		if (distSquared < (KERNEL_HEIGHT * KERNEL_HEIGHT))
		{
			density += LocalParticles[particleCount]->GetModel()->mass * Poly6(distSquared);
		}
	}

	pressure = GAS_CONSTANT * (density - REST_DENSITY);
}

void PhysicsModel::UpdateSPH()
{
	float dist = 0.0f;
	Vector2f diff = Vector2f();
	Vector2f diffNorm = Vector2f();
	Vector2f sumPressure = Vector2f();
	Vector2f sumVisco = Vector2f();

	//P * du/dt = -Dp + uD^2u + Pf
	//-Dp Pressure
	// uD^2u  Viscosity
	//Pf external

	CalculateParticleDensityAndPressure();

	int size = LocalParticles.size();
	for (int particleCount = 0; particleCount < size; particleCount++)
	{
		diff = LocalParticles.at(particleCount)->GetModel()->position - position;
		diffNorm = diff;
		diffNorm.GetNormalized();
		dist = diff.GetLength();

		if (dist != 0.0f)
		{
			if (dist < KERNEL_HEIGHT)
			{
				sumPressure +=
					(-1 * diffNorm) *
					mass *
					(pressure + LocalParticles.at(particleCount)->GetModel()->pressure) / (2.0f * LocalParticles.at(particleCount)->GetModel()->density) *
					Spiky(dist);

				sumVisco +=
					VISCOSITY_CONSTANT *
					mass *
					((LocalParticles.at(particleCount)->GetModel()->velocity - velocity) / LocalParticles.at(particleCount)->GetModel()->density) *
					ViscoKernel(dist);
			}
		}
	}

	netForce = (-1 * sumPressure) + sumVisco + /*EXTERNAL*/ (GRAVITY * mass * density);
}

//Poly6 Kernel
float PhysicsModel::Poly6(float radius_square)
{
	if (radius_square >= 0.0f && radius_square <= KERNEL_HEIGHT)
		return 315.0f / (64.0f * M_PI * pow(KERNEL_HEIGHT, 9)) * pow(KERNEL_HEIGHT * KERNEL_HEIGHT - radius_square, 3);
	else
		return 0.0f;
}

//Colonel Mikey
float PhysicsModel::Spiky(float radius)
{
	if (radius >= 0.0f && radius <= KERNEL_HEIGHT)
		return -15.0f / (M_PI * pow(KERNEL_HEIGHT, 6)) * pow(KERNEL_HEIGHT - radius, 3);
	else
		return 0.0f;
}

//sksksksksk
float PhysicsModel::ViscoKernel(float radius)
{
	if (radius >= 0.0f && radius <= KERNEL_HEIGHT)
		return -15.0f / (2 * M_PI * pow(KERNEL_HEIGHT, 3)) * (0.0f - ((radius * radius * radius) / (2 * (KERNEL_HEIGHT * KERNEL_HEIGHT * KERNEL_HEIGHT))) + ((radius * radius) / (KERNEL_HEIGHT * KERNEL_HEIGHT)) + (KERNEL_HEIGHT / (2 * radius)) - 1);
	else
		return 0.0f;
}

