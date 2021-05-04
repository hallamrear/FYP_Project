#pragma once
#include "Vector.h"

class Particle;

class PhysicsModel
{
private:
	bool isResting;
	Vector2f previousPosition;

	float initialDensity;

public:
	PhysicsModel();
	PhysicsModel(Vector2f position, Vector2f velocity);
	std::vector<Particle*> LocalParticles;

	//TODO : CREATE GETTERS AND SETTERS	
	Vector2f position;
	Vector2f acceleration;
	Vector2f velocity;
	Vector2f externalForce;
	float density;
	float pressure;
	float viscosity;
	float mass;

	void ApplyExternalForce(Vector2f force);
	void Update(float DeltaTime);
	void Reset();
};

