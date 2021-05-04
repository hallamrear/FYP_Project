#include "pch.h"
#include "Simulation.h"
#include "Particle.h"
#include "Collisions.h"
#include "Overloads.h"
#include "PhysicsModel.h"

Simulation::Simulation(
	int particle_count,
	Vector2f world_size,
	Vector2i grid_size)
{
	particleCount = particle_count;

	Vector2f cellSize;
	cellSize.x = world_size.x / grid_size.x;
	cellSize.y = world_size.y / grid_size.y;

	grid = new SpatialGrid(grid_size, cellSize);

	particleSystem = new ParticleSystem(particle_count);

	for (int i = 0; i < STARTING_PARTICLE_COUNT; i++)
	{
		grid->Populate(particleSystem->GetFreshParticle());
	}
}

Simulation::~Simulation()
{

}

float Simulation::CalculateParticlePressure(Particle* particle, std::vector<Particle*>* locals)
{
	float FocussedParticleLocalPressure = GAS_CONSTANT * particle->GetModel()->density;
	//Pressure for particle i = weighted of surrounding
	float pressureSum = 0.0f;
	float dist = 0.0f;
	Vector2f diff;

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
	Vector2f diff;

	//Density Calculation
	int size = locals->size();
	for (int particleCount = 0; particleCount < size; particleCount++)
	{
		if (locals->at(particleCount) != particle)
		{
			diff = locals->at(particleCount)->GetModel()->position - particle->GetModel()->position;
			dist = sqrt((diff.x * diff.x) + (diff.y * diff.y));

			DensitySum += locals->at(particleCount)->GetModel()->mass * Poly6(dist);
		}
	}

	return DensitySum;
}

float Simulation::CalculateParticleViscosity(Particle* particle, std::vector<Particle*>* locals)
{
	float ViscositySum = 0.0f;
	float dist = 0.0f;
	Vector2f diff;
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
	Vector2f diff = Vector2f();
	Vector2f pressureForce = Vector2f();
	Vector2f viscForce = Vector2f();

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
				if (Collisions::CircleInCircle(cell->neighbours[i]->particles[j]->GetModel()->position, PARTICLE_SEARCH_DISTANCE, particle->GetModel()->position, PARTICLE_SEARCH_DISTANCE))
				{
					locals->push_back(cell->neighbours[i]->particles[j]);
				}
			}
		}

		//Check your cells particles too
		for (int k = 0; k < cell->particles.size(); k++)
		{
			if (Collisions::CircleInCircle(cell->particles[k]->GetModel()->position, PARTICLE_SEARCH_DISTANCE, particle->GetModel()->position, PARTICLE_SEARCH_DISTANCE))
			{
				locals->push_back(cell->particles[k]);
			}
		}

		locals->erase(std::unique(locals->begin(), locals->end()), locals->end());
	}
}

void Simulation::AddParticle(Vector2i mouseLocation)
{
	Particle* particle = particleSystem->GetFreshParticle();
	particle->GetModel()->position.x = mouseLocation.x;
	particle->GetModel()->position.y = mouseLocation.y;
}

void Simulation::RemoveParticle(Vector2i mouseLocation)
{
	if(particleSystem->livingParticleCount > 0)
		particleSystem->KillParticle(particleSystem->LivingParticles.back());
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

		particleSystem->LivingParticles[i]->GetModel()->LocalParticles = allLocalParticles;

		for (int neighbours = 0; neighbours < allLocalParticles.size(); neighbours++)
		{
			if(Collisions::CircleInCircle(
				particleSystem->LivingParticles[i]->GetModel()->position, particleSystem->LivingParticles[i]->GetColliderRadius(),
				allLocalParticles[neighbours]->GetModel()->position, allLocalParticles[neighbours]->GetColliderRadius()))
			{
				particleSystem->LivingParticles[i]->ResolveCollision(allLocalParticles[neighbours]);
			}
		}

		particleSystem->LivingParticles[i]->Update(DeltaTime);
	}

}

//Poly6 Kernel
float Simulation::Poly6(float radius_square)
{
	if (radius_square >= 0.0f && radius_square <= KERNEL_HEIGHT)
		return 315.0f / (64.0f * M_PI * pow(KERNEL_HEIGHT, 9)) * pow(KERNEL_HEIGHT * KERNEL_HEIGHT - radius_square, 3);
	else
		return 0.0f;
}

//Colonel Spikey Mikey
float Simulation::Spiky(float radius)
{
	if (radius >= 0.0f && radius <= KERNEL_HEIGHT)
		return 15.0f / (M_PI * pow(KERNEL_HEIGHT, 6)) * pow(KERNEL_HEIGHT - radius, 3);
	else
		return 0.0f;
}

float Simulation::ViscoKernel(float radius)
{
	if (radius >= 0.0f && radius <= KERNEL_HEIGHT)
		return 15.0f / (2 * M_PI * pow(KERNEL_HEIGHT, 3)) * (0.0f - ((radius * radius * radius) / (2 * (KERNEL_HEIGHT * KERNEL_HEIGHT * KERNEL_HEIGHT))) + ((radius * radius) / (KERNEL_HEIGHT * KERNEL_HEIGHT)) + (KERNEL_HEIGHT / (2 * radius)) - 1);
	else
		return 0.0f;
}

void Simulation::Render()
{
	if (particleSystem->livingParticleCount > 0)
	{
		grid->RenderGrid();
		//grid->RenderMarchingSquares();
		particleSystem->Render();
	}

}
