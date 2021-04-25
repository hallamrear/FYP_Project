#include "pch.h"
#include "PhysicsModel.h"
#include "Overloads.h"

PhysicsModel::PhysicsModel()
{
	isResting = false;

	position = XMFLOAT2(123456789.0F, 123456789.0F);
	acceleration = XMFLOAT2(0.0f, 0.0f);
	velocity = XMFLOAT2(0.0f, 0.0f);
	externalForce = XMFLOAT2(0.0f, 0.0f);

	density = 10.0f;
	viscosity = 10.0f;
	mass = 100.0f;
	pressure = 5.0f;

	Reset();
}

PhysicsModel::PhysicsModel(XMFLOAT2 position, XMFLOAT2 velocity = XMFLOAT2(0.0f, 0.0f))
{
	Reset();
	this->position = position;
	this->velocity = velocity;
	acceleration = XMFLOAT2(0.0f, 0.0f);
	externalForce = XMFLOAT2(0.0f, 0.0f);

	density = 10.0f;
	viscosity = 10.0f;
	mass = 100.0f;
	pressure = 5.0f;
}

void PhysicsModel::ApplyExternalForce(XMFLOAT2 force)
{
	externalForce += force;
}

void PhysicsModel::Update(float DeltaTime)
{
	if (position.x < WORLD_EDGE || position.x > WORLD_SIZE.x - WORLD_EDGE)
		velocity.x *= -1;

	if (position.y < WORLD_EDGE || position.y > WORLD_SIZE.y - WORLD_EDGE)
		velocity.y *= -1;

	float dragcoeff = 0.47f;
	float mass = 10.0f;

	XMFLOAT2 netforce = XMFLOAT2(0.0f, 0.0f);
	XMFLOAT2 drag = XMFLOAT2(0.0f, 0.0f);

	///Weight
	//Add weight as a force based on the gravity(mass x g)
	XMFLOAT2 weight = XMFLOAT2(0.0f, 0.0f);

	if(isResting == false)
		weight += GRAVITY;

	weight.x *= mass;
	weight.y *= mass;

	//Calculate components of drag force Stokes' drag
	//drag works in the opposite direction to velocity
	drag.x = -1 * dragcoeff * velocity.x;
	drag.y = -1 * dragcoeff * velocity.y;

	netforce += weight;
	netforce += drag;
	netforce += externalForce;

	if (netforce != XMFLOAT2(0.0f, 0.0f))
	{
		acceleration.x = netforce.x / mass;
		acceleration.y = netforce.y / mass;
	}

	velocity += acceleration;

	position.x += velocity.x * DeltaTime;
	position.y += velocity.y * DeltaTime;

	if (velocity.x < VELOCITY_THRESHOLD)
		velocity.x = 0.0f;
	if (velocity.y < VELOCITY_THRESHOLD)
		velocity.y = 0.0f;

	if (velocity == XMFLOAT2(0.0F, 0.0F))
		isResting = true;
	else
		isResting = false;

	externalForce = XMFLOAT2(0.0f, 0.0f);

	previousPosition = position;
}

void PhysicsModel::Reset()
{
	//TODO : Set all variables to 0
	position = XMFLOAT2(123456789.0F, 123456789.0F);
	acceleration = XMFLOAT2();
	velocity = XMFLOAT2();
	externalForce = XMFLOAT2();

	density = 10.0f;
	viscosity = 10.0f;
	mass = 100.0f;
	pressure = 5.0f;
}
