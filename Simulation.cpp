#include "pch.h"
#include "Simulation.h"
#include "Particle.h"
#include "Collisions.h"
#include "NewRenderer.h"
#include "Overloads.h"

Simulation::Simulation(
	int particle_count,
	float particle_neighbour_distance,
	XMFLOAT2 world_size,
	XMINT2 grid_size)
{
	//Create Renderer
	ParticleRenderer::Get()->Init(NULL, NULL, 0, 0);

	particleCount = particle_count;
	particleNeighbourSearchRadius = particle_neighbour_distance;

	XMFLOAT2 cellSize;
	cellSize.x = world_size.x / grid_size.x;
	cellSize.y = world_size.y / grid_size.y;

	grid = new SpatialGrid(grid_size, cellSize);

	particleSystem = new ParticleSystem(particle_count);

	for (int i = 0; i < particle_count; i++)
	{ 
		grid->Populate(particleSystem->GetFreshParticle());
	}
}

Simulation::~Simulation()
{

}

//Poly6 Kernel
float Simulation::Poly6(float radius_square)
{
	if (radius_square >= 0.0f && radius_square <= kernalHeight)
		return 315.0f / (64.0f * M_PI * pow(kernalHeight, 9)) * pow(kernalHeight * kernalHeight - radius_square, 3);
	else
		return 0.0f;
}

//Colonel Spikey Mikey
float Simulation::Spiky(float radius)
{
	if (radius >= 0.0f && radius <= kernalHeight)
		return 15.0f / (M_PI * pow(kernalHeight, 6)) * pow(kernalHeight - radius, 3);
	else
		return 0.0f;
}

float Simulation::ViscoKernel(float radius)
{
	if (radius >= 0.0f && radius <= kernalHeight)
		return 15.0f / (2 * M_PI * pow(kernalHeight, 3)) * (0.0f - ((radius * radius * radius) / (2 * (kernalHeight * kernalHeight* kernalHeight))) + ((radius * radius) / (kernalHeight * kernalHeight)) + (kernalHeight / (2 * radius)) - 1);
	else
		return 0.0f;
}


float Simulation::CalculateParticlePressure(Particle* particle, std::vector<Particle*>* locals)
{
	float FocussedParticleLocalPressure = GAS_CONSTANT * particle->model.density;
	//Pressure for particle i = weighted of surrounding
	float pressureSum = 0.0f;
	float dist = 0.0f;
	XMFLOAT2 diff;

	//Pressure Calculation
	int size = locals->size();
	for (int particleCount = 0; particleCount < size; particleCount++)
	{
		if (locals->at(particleCount) != particle)
		{
			diff = locals->at(particleCount)->model.position - particle->model.position;
			dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));

			//Using spiky kernel as the poly6 gradient is 0 at the centre
			float localPressure = GAS_CONSTANT * locals->at(particleCount)->model.density;
			pressureSum += particle->model.mass * ((FocussedParticleLocalPressure + localPressure) / (2 * locals->at(particleCount)->model.density) * Spiky(dist));
		}
	}

	return pressureSum;
}

float Simulation::CalculateParticleDensity(Particle* particle, std::vector<Particle*>* locals)
{
	float DensitySum = 0.0f;
	float dist = 0.0f;
	XMFLOAT2 diff;

	//Density Calculation
	int size = locals->size();
	for (int particleCount = 0; particleCount < size; particleCount++)
	{
		if (locals->at(particleCount) != particle)
		{
			diff = locals->at(particleCount)->model.position - particle->model.position;
			dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));

			DensitySum += locals->at(particleCount)->model.mass * Poly6(dist * dist);
		}
	}

	return DensitySum;
}

float Simulation::CalculateParticleViscosity(Particle* particle, std::vector<Particle*>* locals)
{
	float ViscositySum = 0.0f;
	float dist = 0.0f;
	XMFLOAT2 diff;
	float mew = 0.0f;

	//Viscosity Calculation
	int size = locals->size();
	for (int particleCount = 0; particleCount < size; particleCount++)
	{
		diff = locals->at(particleCount)->model.position - particle->model.position;
		dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));
	}

	return mew * ViscositySum;
}

void Simulation::GetLocalParticlesFromGrid(std::vector<Particle*>* locals, Particle* particle)
{
	GridCell* cell = grid->GetCellContainingParticle(particle);

	if (cell != nullptr)
	{
		//In each neighbour
		for (int i = 0; i < cell->neighbours.size(); i++)
		{
			//Check all their particles if theyre in the smoothing radius
			for (int j = 0; j < cell->neighbours[i]->particles.size(); j++)
			{
				if (Collisions::IsPointInCircle(cell->neighbours[i]->particles[j]->model.position, particle->model.position, particleNeighbourSearchRadius))
					locals->push_back(cell->neighbours[i]->particles[j]);
			}
		}

		//Check your cells particles too
		for (int k = 0; k < cell->particles.size(); k++)
		{
			if (Collisions::IsPointInCircle(cell->particles[k]->model.position, particle->model.position, particleNeighbourSearchRadius))
				locals->push_back(cell->particles[k]);
		}
	}
}

void Simulation::Update(float DeltaTime)
{
	grid->ClearCells();

	for (int i = 0; i < particleCount; i++)
	{
		//Populating grid with particles for this frame.
		//grid->Populate(particleSystem->ParticlePool[i]);
	}

	//P * du/dt = -Dp + uD^2u + Pf
	//-Dp Pressure
	// uD^2u  Viscosity
	//Pf external

	/*
	CalculateParticleDensity();
	CalculateParticlePressure();
	CalculateParticleViscosity();
	ApplyForcesToParticles();
	*/

	std::vector<Particle*> allLocalParticles;

	for (int i = 0; i < particleCount; i++)
	{
		allLocalParticles.clear();

		if (particleSystem->ParticlePool[i]->GetAlive() == true)
		{
			//GetLocalParticlesFromGrid(&allLocalParticles, particleSystem->ParticlePool[i]);

			/*float tempDensity = CalculateParticleDensity(particleSystem->ParticlePool[i], &allLocalParticles);
			float tempPressure = CalculateParticlePressure(particleSystem->ParticlePool[i], &allLocalParticles);
			float tempViscosity = CalculateParticleViscosity(particleSystem->ParticlePool[i], &allLocalParticles);*/

			particleSystem->ParticlePool[i]->Update(DeltaTime);
		}
	}

	//Call only seems to update instances
	particleSystem->Update(DeltaTime);
}

void Simulation::Render()
{
	particleSystem->Render();
}
