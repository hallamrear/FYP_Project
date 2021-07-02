#include "pch.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "Renderer.h"

ParticleSystem::ParticleSystem(int MaxParticleCount)
{
    deadParticleCount = 0;
    livingParticleCount = 0;

    this->MaxParticleCount = MaxParticleCount;

    Particle* particle;

    for (int i = 0; i < MaxParticleCount; i++)
    {
        particle = new Particle();
        particle->poolID = i;
        deadParticleCount++;
        deadParticlesList = LinkedList::PushFront(deadParticlesList, particle);
        ParticlePool.push_back(particle);
    }

    particle = nullptr;
}

ParticleSystem::~ParticleSystem()
{

}

Particle* ParticleSystem::GetFreshRandomParticle()
{
    if (deadParticleCount > 0)
    {
        Particle* particle = deadParticlesList->data;
        particle->CreateRandom();
        deadParticlesList = LinkedList::DeleteFront(deadParticlesList);
        LivingParticles.push_back(particle);
        livingParticleCount++;
        deadParticleCount--;
        return particle;
    }
    else
    {
        int num = rand() % livingParticleCount;

        Particle* particle = LivingParticles[num];
        particle->Kill();
        particle->Create();

        return particle;
    }

    return nullptr;
}

Particle* ParticleSystem::GetFreshParticle()
{
    if (deadParticleCount > 0)
    {
        Particle* particle = deadParticlesList->data;
		particle->Create();
        deadParticlesList = LinkedList::DeleteFront(deadParticlesList);
		LivingParticles.push_back(particle);
        livingParticleCount++;
        deadParticleCount--;
        return particle;
    }
	else
	{
		int num = rand() % livingParticleCount;

		Particle* particle = LivingParticles[num];
		particle->Kill();
		particle->Create();

		return particle;
	}

    return nullptr;
}

void ParticleSystem::KillParticle(Particle* particle)   
{
    if (livingParticleCount > 0)
    {
        particle->Kill();
        deadParticlesList = LinkedList::PushFront(deadParticlesList, particle);

        int id = -1;

        for (int i = 0; i < livingParticleCount; i++)
        {
            if (LivingParticles[i] == particle)
                id = i;
        }

        LivingParticles.erase(LivingParticles.begin() + id);

        livingParticleCount--;
        deadParticleCount++;
    }
}

void ParticleSystem::Update(float DeltaTime)
{

}

void ParticleSystem::Render()
{
	for(int i = 0; i < livingParticleCount; i++)
		Renderer::RenderParticle(LivingParticles[i]);
}
