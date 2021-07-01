#pragma once
#include "SimConsts.h"
#include "SpatialGrid.h"
#include "ParticleSystem.h"

enum RENDER_STATE
{
	RENDER_PARTICLE_SYSTEM = 0,
};

class PhysicsModel;

class Simulation
{
	bool isRunning;

	//Simulation Variables
	int particleCount;

	RENDER_STATE renderMode;
	ParticleSystem* particleSystem;
	SpatialGrid* grid;

	void GetLocalParticlesFromGrid(std::vector<Particle*>* local, Particle* particle);

public:
	Simulation(int particle_count , Vector2f world_size, Vector2i grid_size);
	~Simulation();

	void ResetSimulation();
	void ToggleIsRunning();

	Particle* AddParticle(Vector2i mouseLocation);
	void RemoveParticle(Vector2i mouseLocation);
	void Update(float DeltaTime);
	void Render();
};

