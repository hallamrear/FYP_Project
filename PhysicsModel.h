#pragma once
#define GRAVITY XMFLOAT2(0.0f, 9.81f)
#define VELOCITY_THRESHOLD 1.0f
#define GAS_CONSTANT 8.314

class PhysicsModel
{
private:
	bool isResting;
	XMFLOAT2 previousPosition;

	float initialDensity;


public:
	PhysicsModel();
	PhysicsModel(XMFLOAT2 position, XMFLOAT2 velocity);

	//TODO : CREATE GETTERS AND SETTERS	
	XMFLOAT2 position;
	XMFLOAT2 acceleration;
	XMFLOAT2 velocity;
	XMFLOAT2 externalForce;
	float density;
	float pressure;
	float viscosity;
	float mass;

	void ApplyExternalForce(XMFLOAT2 force);
	void Update(float DeltaTime);
	void Reset();
};

