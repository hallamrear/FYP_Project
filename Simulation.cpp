#include "pch.h"
#include "Simulation.h"
#include "Particle.h"
#include "Collisions.h"
#include "Overloads.h"
#include "PhysicsModel.h"

Simulation::Simulation(
	int particle_count,
	float particle_neighbour_distance,
	XMFLOAT2 world_size,
	XMINT2 grid_size)
{

	//top
	walls[0].max.x = WORLD_SIZE.x;
	walls[0].max.y = 0.0f;
	walls[0].min.x = 0.0f;
	walls[0].min.y = WORLD_EDGE;
	//bottom
	walls[1].max.x = WORLD_SIZE.x;
	walls[1].max.y = WORLD_EDGE;
	walls[1].min.x = 0.0f;
	walls[1].min.y = WORLD_SIZE.y;
	//left
	walls[2].max.x = WORLD_EDGE;
	walls[2].max.y = 0.0f;
	walls[2].min.x = 0.0f;
	walls[2].min.y = WORLD_SIZE.y;
	//right
	walls[3].max.x = WORLD_SIZE.x;
	walls[3].max.y = 0.0f;
	walls[3].min.x = WORLD_SIZE.x - WORLD_EDGE;
	walls[3].min.y = WORLD_SIZE.y;

	particleCount = particle_count;
	particleNeighbourSearchRadius = particle_neighbour_distance;

	XMFLOAT2 cellSize;
	cellSize.x = world_size.x / grid_size.x;
	cellSize.y = world_size.y / grid_size.y;

	grid = new SpatialGrid(grid_size, cellSize);

	particleSystem = new ParticleSystem(particle_count);

	for (int i = 0; i < particle_count; i++)
	{
		particleSystem->GetFreshParticle();
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
	float FocussedParticleLocalPressure = GAS_CONSTANT * particle->GetModel()->density;
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
			diff = locals->at(particleCount)->GetModel()->position - particle->GetModel()->position;
			dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));

			//Using spiky kernel as the poly6 gradient is 0 at the centre
			float localPressure = GAS_CONSTANT * locals->at(particleCount)->GetModel()->density;
			pressureSum += particle->GetModel()->mass * ((FocussedParticleLocalPressure + localPressure) / (2 * locals->at(particleCount)->GetModel()->density) * Spiky(dist));
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
			diff = locals->at(particleCount)->GetModel()->position - particle->GetModel()->position;
			dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));

			DensitySum += locals->at(particleCount)->GetModel()->mass * Poly6(dist * dist);
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
		diff = locals->at(particleCount)->GetModel()->position - particle->GetModel()->position;
		dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));
	}

	return mew * ViscositySum;
}

void Simulation::ApplyForceToParticle(Particle* particle, std::vector<Particle*>* locals)
{

	float dist = 0.0f;
	XMFLOAT2 diff = XMFLOAT2();
	XMFLOAT2 pressureForce = XMFLOAT2();
	XMFLOAT2 viscForce = XMFLOAT2();

	int size = locals->size();
	for (int particleCount = 0; particleCount < size; particleCount++)
	{
		diff = locals->at(particleCount)->GetModel()->position - particle->GetModel()->position;
		dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));

		if (dist != 0.0f)
		{
			diff.x /= dist;
			diff.y /= dist;


			pressureForce += diff * particle->GetModel()->mass * (particle->GetModel()->pressure);

			viscForce += VISCOSITY_CONSTANT * particle->GetModel()->mass *
				((locals->at(particleCount)->GetModel()->velocity - particle->GetModel()->velocity) / locals->at(particleCount)->GetModel()->pressure) *
				ViscoKernel(dist);
		}
	}

	particle->GetModel()->ApplyExternalForce(pressureForce);
	particle->GetModel()->ApplyExternalForce(viscForce);


}

void Simulation::GetLocalParticlesFromGrid(std::vector<Particle*>* locals, Particle* particle)
{
	GridCell* cell = grid->GetCellContainingParticle(particle);

	if (cell != nullptr)
	{
		//In each neighbour
		for (int i = 0; i < cell->neighbours.size(); i++)
		{
			if (cell->neighbours[i] == cell)
				continue;

			//Check all their particles if theyre in the smoothing radius
			for (int j = 0; j < cell->neighbours[i]->particles.size(); j++)
			{
				if (Collisions::IsPointInCircle(cell->neighbours[i]->particles[j]->GetModel()->position, particle->GetModel()->position, particleNeighbourSearchRadius))
				{
					locals->push_back(cell->neighbours[i]->particles[j]);
				}
			}
		}

		//Check your cells particles too
		for (int k = 0; k < cell->particles.size(); k++)
		{
			if (Collisions::IsPointInCircle(cell->particles[k]->GetModel()->position, particle->GetModel()->position, particleNeighbourSearchRadius))
			{
					locals->push_back(cell->particles[k]);
			}
		}

		locals->erase(std::unique(locals->begin(), locals->end()), locals->end());
	}
}

void Simulation::AddParticle(XMINT2 mouseLocation)
{
	Particle* particle = particleSystem->GetFreshParticle();
	particle->GetModel()->position.x = mouseLocation.x;
	particle->GetModel()->position.y = mouseLocation.y;
}

void Simulation::Update(float DeltaTime)
{
	grid->ClearCells();

	for (int i = 0; i < particleSystem->livingParticleCount; i++)
	{
		//Populating grid with particles for this frame.
		grid->Populate(particleSystem->LivingParticles[i]);
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

	for (int i = 0; i < particleSystem->livingParticleCount; i++)
	{
		allLocalParticles.clear();

		GetLocalParticlesFromGrid(&allLocalParticles, particleSystem->LivingParticles[i]);

		particleSystem->LivingParticles[i]->GetModel()->density = CalculateParticleDensity(particleSystem->LivingParticles[i], &allLocalParticles);
		particleSystem->LivingParticles[i]->GetModel()->pressure = CalculateParticlePressure(particleSystem->LivingParticles[i], &allLocalParticles);
		particleSystem->LivingParticles[i]->GetModel()->viscosity = CalculateParticleViscosity(particleSystem->LivingParticles[i], &allLocalParticles);

		for (int neighbours = 0; neighbours < allLocalParticles.size(); neighbours++)
		{
			if(c2CircletoCircle(particleSystem->LivingParticles[i]->GetCollider(), allLocalParticles[neighbours]->GetCollider()))
			{
				particleSystem->LivingParticles[i]->ResolveCollision(allLocalParticles[neighbours]);
			}
		}

		//ApplyForceToParticle(particleSystem->LivingParticles[i], &allLocalParticles);

		particleSystem->LivingParticles[i]->Update(DeltaTime);
	}

	//Call only seems to update instances
	//particleSystem->Update(DeltaTime);
}

void Simulation::Render()
{
	particleSystem->Render();
}
