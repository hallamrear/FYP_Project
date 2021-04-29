#pragma once
#include "SpatialGrid.h"
#include "ParticleSystem.h"
#include "cute_c2.h"

enum RENDER_STATE
{
	RENDER_PARTICLE_SYSTEM = 0,
};


class Simulation
{
	//TODO : Create
	c2AABB walls[4];
	
	//Simulation Variables
	int particleCount;
	float particleNeighbourSearchRadius;
	//KERNEL STUFF TODO : find references for kernel data
	float kernalHeight = 0.04f;
	float Poly6(float radius_square);
	float Spiky(float radius);
	float ViscoKernel(float radius);

	RENDER_STATE renderMode;
	ParticleSystem* particleSystem;
	SpatialGrid* grid;

	float CalculateParticleDensity(Particle* particle, std::vector<Particle*>* locals);
	float CalculateParticlePressure(Particle* particle, std::vector<Particle*>* locals);
	float CalculateParticleViscosity(Particle* particle, std::vector<Particle*>* locals);
	void ApplyForceToParticle(Particle* particle, std::vector<Particle*>* locals);

	void GetLocalParticlesFromGrid(std::vector<Particle*>* local, Particle* particle);

public:
	Simulation(int particle_count, float particle_neighbour_distance, XMFLOAT2 world_size, XMINT2 grid_size);
	~Simulation();


	void AddParticle(XMINT2 mouseLocation);
	void Update(float DeltaTime);
	void Render();
};

