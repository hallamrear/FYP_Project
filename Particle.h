#pragma once
#include "PhysicsModel.h"
#include "cute_c2.h"

class Particle
{
private:
	static ID3D11Resource* textureResource;
	static ID3D11ShaderResourceView* texture;
	c2Circle circleCollider;
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
	c2Circle GetCollider();
	void ResolveCollision(Particle* particle);

	void Update(float DeltaTime);

	ID3D11ShaderResourceView* GetTexture();
};

