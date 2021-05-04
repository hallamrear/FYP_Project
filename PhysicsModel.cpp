#include "pch.h"
#include "PhysicsModel.h"
#include "Overloads.h"
#include "Particle.h"

PhysicsModel::PhysicsModel()
{
	isResting = false;

	position = Vector2f(123456789.0F, 123456789.0F);
	acceleration = Vector2f(0.0f, 0.0f);
	velocity = Vector2f(0.0f, 0.0f);
	externalForce = Vector2f(0.0f, 0.0f);

	initialDensity = density = 10.0f;
	viscosity = 10.0f;
	mass = 100.0f;
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
	mass = 100.0f;
	pressure = 5.0f;
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
			position.x = WORLD_EDGE + 0.1f;
		else
			position.x = WORLD_SIZE.x - WORLD_EDGE - 0.1f;

		velocity.x *= -0.95f;
	}

	if (position.y < WORLD_EDGE || position.y > WORLD_SIZE.y - WORLD_EDGE)
	{
		if (position.y < WORLD_EDGE)
			position.y = WORLD_EDGE + 0.1f;
		else
			position.y = WORLD_SIZE.y - WORLD_EDGE - 0.1f;

		velocity.y *= -0.95f;
	}

	Vector2f netforce = Vector2f(0.0f, 0.0f);

	///Weight
	//Add weight as a force based on the gravity(mass x g)
	Vector2f weight = Vector2f(0.0f, 0.0f);

	if (isResting == false)
		weight += GRAVITY;
	else
		weight += Vector2f(0.0f, 0.0f);

	weight.x *= mass;
	weight.y *= mass;

	netforce += weight;
	netforce += externalForce;

	if (netforce != Vector2f(0.0f, 0.0f))
	{
		acceleration.x = netforce.x / mass;
		acceleration.y = netforce.y / mass;
	}

	velocity += acceleration;

	position.x += velocity.x * DeltaTime;
	position.y += velocity.y * DeltaTime;
	
	if (velocity.x < VELOCITY_THRESHOLD && velocity.x > (-1 * VELOCITY_THRESHOLD))
		velocity.x = 0.0f;
	if (velocity.y < VELOCITY_THRESHOLD && velocity.y > (-1 * VELOCITY_THRESHOLD))
		velocity.y = 0.0f;

	if (velocity == Vector2f(0.0F, 0.0F))
		isResting = true;
	else
		isResting = false;

	externalForce = Vector2f(0.0f, 0.0f);

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
	mass = 100.0f;
	pressure = 5.0f;
}
