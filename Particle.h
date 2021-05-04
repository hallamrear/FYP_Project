#pragma once
#include "PhysicsModel.h"

class Particle
{
private:
	float colliderRadius;
	PhysicsModel model;

public:
	UINT poolID;
	bool isAlive;

	Particle();
	~Particle();

	void Create();
	void CreateRandom();
	void Kill();

	void SetAlive(bool);
	bool GetAlive();

	PhysicsModel* GetModel();
	void ResolveCollision(Particle* particle);
	float GetColliderRadius();

	void Update(float DeltaTime);
};

