#pragma once
#include "LinkedList.h"
class Particle;

class ParticleSystem
{
private:
	static ParticleSystem* instance;

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* InstanceBuffer;

	//Linked list of particles which are dead and can be reallocated
	LinkedListNode* deadParticlesList;


	HRESULT CreateBuffers();
	void UpdateInstanceBuffer();

public:
	//Exposing to simulation so i dont have to loop twice
	std::vector<Particle*> ParticlePool;
	std::vector<Particle*> LivingParticles;

	UINT MaxParticleCount;
	UINT livingParticleCount;
	UINT deadParticleCount;

	ParticleSystem(int MaxParticleCount);
	~ParticleSystem();

	Particle* GetFreshParticle();
	void KillParticle(Particle* particle);

	void Update(float DeltaTime);

	void Render();

};

