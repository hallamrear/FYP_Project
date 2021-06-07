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

	isRunning = true;
}

Simulation::~Simulation()
{

}

void Simulation::ToggleIsRunning()
{
	isRunning = !isRunning;
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
				if (Collisions::CircleInCircle(cell->neighbours[i]->particles[j]->GetModel()->GetPosition(), PARTICLE_SEARCH_DISTANCE, particle->GetModel()->GetPosition(), PARTICLE_SEARCH_DISTANCE))
				{
					locals->push_back(cell->neighbours[i]->particles[j]);
				}
			}
		}

		//Check your cells particles too
		for (int k = 0; k < cell->particles.size(); k++)
		{
			if (Collisions::CircleInCircle(cell->particles[k]->GetModel()->GetPosition(), PARTICLE_SEARCH_DISTANCE, particle->GetModel()->GetPosition(), PARTICLE_SEARCH_DISTANCE))
			{
				locals->push_back(cell->particles[k]);
			}
		}

		locals->erase(std::unique(locals->begin(), locals->end()), locals->end());
	}
}

Particle* Simulation::AddParticle(Vector2i mouseLocation)
{
	Particle* particle = particleSystem->GetFreshParticle();
	Vector2f pos = Vector2f((float)mouseLocation.x, (float)mouseLocation.y);
	particle->GetModel()->SetPosition(pos);
	return particle;
}

void Simulation::RemoveParticle(Vector2i mouseLocation)
{
	if(particleSystem->livingParticleCount > 0)
		particleSystem->KillParticle(particleSystem->LivingParticles.back());
}

void Simulation::Update(float DeltaTime)
{
	if (isRunning == false)
		return;

	grid->ClearCells();

	for (int i = 0; i < particleSystem->livingParticleCount; i++)
	{
		//Populating grid with particles for this frame.
		grid->Populate(particleSystem->LivingParticles[i]);
	}

	std::vector<Particle*> allLocalParticles;

	for (int i = 0; i < particleSystem->livingParticleCount; i++)
	{
		allLocalParticles.clear();

		GetLocalParticlesFromGrid(&allLocalParticles, particleSystem->LivingParticles[i]);

		particleSystem->LivingParticles[i]->GetModel()->LocalParticles = allLocalParticles;

		particleSystem->LivingParticles[i]->Update(DeltaTime);

		for (int neighbours = 0; neighbours < allLocalParticles.size(); neighbours++)
		{
			if(Collisions::CircleInCircle(
				particleSystem->LivingParticles[i]->GetModel()->GetPosition(), particleSystem->LivingParticles[i]->GetColliderRadius(),
				allLocalParticles[neighbours]->GetModel()->GetPosition(), allLocalParticles[neighbours]->GetColliderRadius()))
			{
				particleSystem->LivingParticles[i]->ResolveCollision(allLocalParticles[neighbours]);
			}
		}

	}

}

void Simulation::Render()
{
	grid->RenderGrid();
	particleSystem->Render();

	if (particleSystem->livingParticleCount > 0)
	{		
		grid->RenderMarchingSquares();
	}
}
