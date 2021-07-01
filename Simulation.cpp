#include "pch.h"
#include "Simulation.h"
#include "Particle.h"
#include "Collisions.h"
#include "Overloads.h"
#include "PhysicsModel.h"
#include "Renderer.h"

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

	ResetSimulation();

	isRunning = true;
}

Simulation::~Simulation()
{

}

void Simulation::ResetSimulation()
{
	for (int i = 0; i < particleSystem->livingParticleCount; i++)
	{
		particleSystem->KillParticle(particleSystem->LivingParticles[i]);
	}

	for (float y = WORLD_EDGE; y < WORLD_SIZE.y - WORLD_EDGE * 2.f; y += KERNEL_HEIGHT)
	{
		for (float x = WORLD_SIZE.x / 4; x <= WORLD_SIZE.x / 2; x += KERNEL_HEIGHT)
		{
			if (particleSystem->livingParticleCount < STARTING_PARTICLE_COUNT)
			{
				float jitter = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				Particle* p = particleSystem->GetFreshParticle();
				p->GetModel()->SetPosition(Vector2f(x + jitter, y));
				grid->Populate(p);
			}
		}
	}
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


		if (isRunning)
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

	if (isRunning == false)
	{
		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(50.0f, 50.0f));
		rect.setOrigin(sf::Vector2f(25.0f, 25.0f));
		rect.setFillColor(sf::Color::Red);
		rect.setOutlineColor(sf::Color::Green);
		rect.setPosition(WORLD_SIZE.x / 2, WORLD_SIZE.y / 2);
		GraphicsDevice::GetWindow()->draw(rect);
	}
}