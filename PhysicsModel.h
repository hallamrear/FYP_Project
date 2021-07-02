#pragma once
#include "Vector.h"

class Particle;

class PhysicsModel
{
private:
	bool isResting;
	Vector2f previousPosition;
	Vector2f position;
	Vector2f acceleration;
	Vector2f velocity;
	Vector2f externalForce;
	float pressure;
	float viscosity;
	float mass;
	float nearDensity;

	float initialDensity;

	float Poly6(float radius_square);
	float Spiky(float radius);
	float ViscoKernel(float radius);

	void CalculateDoubleDensity(float DeltaTime);
	void CalculatePressure();
	void CalculateViscosity(float DeltaTime);

	void EnforceEdges();

public:
	float density;

	Vector2f netForce;

	PhysicsModel();
	PhysicsModel(Vector2f position, Vector2f velocity);
	std::vector<Particle*> LocalParticles;

	Vector2f GetPosition();
	void SetPosition(Vector2f position);
	Vector2f GetVelocity();
	void SetVelocity(Vector2f velocity);
	float GetMass();
	void SetMass(float mass);
	void UpdateSPH(float DeltaTime);

	bool IsResting();

	void ApplyExternalForce(Vector2f force);
	void Reset();
};

