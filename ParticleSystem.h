#pragma once
#include "LinkedList.h"
class Particle;

class ParticleSystem
{
private:
	//Linked list of particles which are dead and can be reallocated
	LinkedListNode* deadParticlesList;
	int isRenderingDetails = 0;

public:
	//Exposing to simulation so i dont have to loop twice
	std::vector<Particle*> ParticlePool;
	std::vector<Particle*> LivingParticles;

	UINT MaxParticleCount;
	UINT livingParticleCount;
	UINT deadParticleCount;

	void SetParticleRenderDetail(int state);

	ParticleSystem(int MaxParticleCount);
	~ParticleSystem();

	Particle* GetFreshRandomParticle();
	Particle* GetFreshParticle();
	void KillParticle(Particle* particle);

	void Update(float DeltaTime);
	void Render();

};

