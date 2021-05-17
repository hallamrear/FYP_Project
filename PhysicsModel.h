#pragma once
#include "Vector.h"

class Particle;

class PhysicsModel
{
private:
	bool isResting;
	Vector2f previousPosition;
	Vector2f netForce;
	Vector2f position;
	Vector2f acceleration;
	Vector2f velocity;
	Vector2f externalForce;
	float density;
	float pressure;
	float viscosity;
	float mass;

	float initialDensity;

	float Poly6(float radius_square);
	float Spiky(float radius);
	float ViscoKernel(float radius);

	void CalculateParticleDensityAndPressure();
	void UpdateSPH();

public:
	PhysicsModel();
	PhysicsModel(Vector2f position, Vector2f velocity);
	std::vector<Particle*> LocalParticles;

	Vector2f GetPosition();
	void SetPosition(Vector2f position);
	Vector2f GetVelocity();
	void SetVelocity(Vector2f velocity);
	float GetMass();
	void SetMass(float mass);

	bool IsResting();

	void ApplyExternalForce(Vector2f force);
	void Update(float DeltaTime);
	void Reset();
};

