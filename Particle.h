#pragma once
#include "PhysicsModel.h"
#include "SimConsts.h"

class alignas(16) Particle
{	
private:
	PhysicsModel model;
	float colliderRadius;
public:
	UINT poolID;
	bool isAlive;
	bool isStatic;
private:
	bool Padding[6];
public:

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

