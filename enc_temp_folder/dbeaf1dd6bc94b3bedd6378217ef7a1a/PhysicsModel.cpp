#include "pch.h"
#include "PhysicsModel.h"
#include "Overloads.h"
#include "Particle.h"

void PhysicsModel::ApplyForce(Vector2f force)
{
	netForce += force;
}

PhysicsModel::PhysicsModel()
{
	isResting = false;

	position = Vector2f(123456789.0F, 123456789.0F);
	acceleration = Vector2f(0.0f, 0.0f);
	velocity = Vector2f(0.0f, 0.0f);
	externalForce = Vector2f(0.0f, 0.0f);

	initialDensity = density = 10.0f;
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

	density = 10.0f;
	viscosity = 10.0f;
	mass = DEFAULT_PARTICLE_MASS;
	pressure = 5.0f;
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

		velocity.x *= -0.9f;
	}

	if (position.y < WORLD_EDGE || position.y > WORLD_SIZE.y - WORLD_EDGE)
	{
		if (position.y < WORLD_EDGE)
			position.y = WORLD_EDGE;
		else
			position.y = WORLD_SIZE.y - WORLD_EDGE;

		velocity.y *= -0.9f;
	}

	//Calculate components of drag force Stokes' drag
	//drag works in the opposite direction to velocity
	Vector2f drag;
	drag.x = -1 * DEFAULT_PARTICLE_DRAG_COEFFICIENT * velocity.x;
	drag.y = -1 * DEFAULT_PARTICLE_DRAG_COEFFICIENT * velocity.y;

	///Weight
	//Add weight as a force based on the gravity(mass x g)
	Vector2f weight = Vector2f(0.0f, 0.0f);

	if (isResting == false)
		weight += GRAVITY;
		//weight += Vector2f(0.0f, 0.0f);
	else
		weight += Vector2f(0.0f, 0.0f);

	weight.x *= density;
	weight.y *= density;

	ApplyForce(weight);
	//ApplyForce(drag);
	CalculateSPH();
	ApplyForce(externalForce);


	acceleration.x = netForce.x / mass;
	acceleration.y = netForce.y / mass;

	velocity += acceleration;
	
	if (velocity.GetLength() < REST_LENGTH)
	{
		isResting = true;
		velocity.x = 0.0f;
		velocity.y = 0.0f;
	}
	else
	{
		isResting = false;
		position.x += (velocity.x * DeltaTime);
		position.y += (velocity.y * DeltaTime);
	}

	externalForce = Vector2f(0.0f, 0.0f);
	netForce = Vector2f(0.0f, 0.0f);

	previousPosition = position;
}

void PhysicsModel::Reset()
{
	//TODO : Set all variables to 0
	position = Vector2f(123456789.0F, 123456789.0F);
	acceleration = Vector2f(0.0f, 0.0f);
	velocity = Vector2f(0.0f, 0.0f);
	externalForce = Vector2f(0.0f, 0.0f);

	initialDensity = density = 10.0f;
	viscosity = 10.0f;
	mass = DEFAULT_PARTICLE_MASS;
	pressure = 5.0f;
}


float PhysicsModel::CalculateParticlePressure()
{
	float FocussedParticleLocalPressure = GAS_CONSTANT * density;
	//Pressure for particle i = weighted of surrounding
	float pressureSum = 0.0f;
	float dist = 0.0f;
	Vector2f diff;

	//Pressure Calculation
	int size = LocalParticles.size();
	for (int particleCount = 0; particleCount < size; particleCount++)
	{

			diff = LocalParticles.at(particleCount)->GetModel()->position - position;
			dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));

			//Using spiky kernel as the poly6 gradient is 0 at the centre
			float localPressure = GAS_CONSTANT * LocalParticles.at(particleCount)->GetModel()->density;
			pressureSum += mass * ((FocussedParticleLocalPressure + localPressure) / (2 * LocalParticles.at(particleCount)->GetModel()->density) * Spiky(dist));
	}

	return pressureSum;
}

float PhysicsModel::CalculateParticleDensity()
{
	float DensitySum = 0.0f;
	float dist = 0.0f;
	Vector2f diff;

	//Density Calculation
	int size = LocalParticles.size();
	for (int particleCount = 0; particleCount < size; particleCount++)
	{
			diff = LocalParticles.at(particleCount)->GetModel()->position - position;
			dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));

			DensitySum += LocalParticles.at(particleCount)->GetModel()->mass * Poly6(dist);
	}

	return DensitySum;
}

float PhysicsModel::CalculateParticleViscosity()
{
	float ViscositySum = 0.0f;
	float dist = 0.0f;
	Vector2f diff;
	float mew = 0.0f;

	//Viscosity Calculation
	int size = LocalParticles.size();
	for (int particleCount = 0; particleCount < size; particleCount++)
	{
		diff = LocalParticles.at(particleCount)->GetModel()->position - position;
		dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));
	}

	return mew * ViscositySum;
}

void PhysicsModel::CalculateSPH()
{
	float dist = 0.0f;
	Vector2f diff = Vector2f();
	Vector2f pressureForce = Vector2f();
	Vector2f viscForce = Vector2f();

	int size = LocalParticles.size();
	for (int particleCount = 0; particleCount < size; particleCount++)
	{
		diff = LocalParticles.at(particleCount)->GetModel()->position - position;
		dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));

		if (dist != 0.0f)
		{
			diff.x /= dist;
			diff.y /= dist;

			pressureForce += diff * mass * (pressure);

			viscForce += VISCOSITY_CONSTANT * mass *
				((LocalParticles.at(particleCount)->GetModel()->velocity - velocity) / LocalParticles.at(particleCount)->GetModel()->pressure) *
				ViscoKernel(dist);
		}
	}

	ApplyForce(pressureForce);
	ApplyForce(viscForce);
}

//Poly6 Kernel
float PhysicsModel::Poly6(float radius_square)
{
	if (radius_square >= 0.0f && radius_square <= KERNEL_HEIGHT)
		return 315.0f / (64.0f * M_PI * pow(KERNEL_HEIGHT, 9)) * pow(KERNEL_HEIGHT * KERNEL_HEIGHT - radius_square, 3);
	else
		return 0.0f;
}

//Colonel Spikey Mikey
float PhysicsModel::Spiky(float radius)
{
	if (radius >= 0.0f && radius <= KERNEL_HEIGHT)
		return 15.0f / (M_PI * pow(KERNEL_HEIGHT, 6)) * pow(KERNEL_HEIGHT - radius, 3);
	else
		return 0.0f;
}

//sksksksksk
float PhysicsModel::ViscoKernel(float radius)
{
	if (radius >= 0.0f && radius <= KERNEL_HEIGHT)
		return 15.0f / (2 * M_PI * pow(KERNEL_HEIGHT, 3)) * (0.0f - ((radius * radius * radius) / (2 * (KERNEL_HEIGHT * KERNEL_HEIGHT * KERNEL_HEIGHT))) + ((radius * radius) / (KERNEL_HEIGHT * KERNEL_HEIGHT)) + (KERNEL_HEIGHT / (2 * radius)) - 1);
	else
		return 0.0f;
}

