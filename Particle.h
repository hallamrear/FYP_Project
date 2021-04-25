#pragma once
#include "PhysicsModel.h"

class Particle
{
private:
	static ID3D11Resource* textureResource;
	static ID3D11ShaderResourceView* texture;

public:
	UINT poolID;
	bool isAlive;

	PhysicsModel model;

	Particle();
	~Particle();

	void Create();
	void CreateRandom();
	void Kill();

	void SetAlive(bool);
	bool GetAlive();

	void Update(float DeltaTime);

	ID3D11ShaderResourceView* GetTexture();
};

