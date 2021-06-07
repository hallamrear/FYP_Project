#pragma once
#include "SpatialGrid.h"
#include "ParticleSystem.h"

enum RENDER_STATE
{
	RENDER_PARTICLE_SYSTEM = 0,
};

class Simulation
{
	bool isRunning;

	//Simulation Variables
	int particleCount;

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
	Simulation(int particle_count , Vector2f world_size, Vector2i grid_size);
	~Simulation();

	void ToggleIsRunning();

	Particle* AddParticle(Vector2i mouseLocation);
	void RemoveParticle(Vector2i mouseLocation);
	void Update(float DeltaTime);
	void Render();
};

