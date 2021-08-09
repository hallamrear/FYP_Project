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

	Vector2f marchingSize;
	marchingSize.x = world_size.x / 80;
	marchingSize.y = world_size.y / 45;

	grid = new SpatialGrid(grid_size, cellSize);
	marchingGrid = new SpatialGrid(Vector2i(160, 90), marchingSize);

	particleSystem = new ParticleSystem(particle_count);

	ResetSimulation();

	isRunning = true;
	isRenderingDetails = false;
}

Simulation::~Simulation()
{

}

const ParticleSystem* const Simulation::GetParticleSystem()
{
	return particleSystem;
}

void Simulation::ResetSimulation()
{
	for (int i = 0; i < particleSystem->livingParticleCount; i++)
	{
		particleSystem->KillParticle(particleSystem->LivingParticles[i]);
	}	

	for (int X = WORLD_EDGE; X < WORLD_SIZE.x - WORLD_EDGE; X += PARTICLE_COLLIDER_SIZE * 2)
	{
		Particle* pBot = AddParticle(Vector2i(X, WORLD_EDGE));
		pBot->isStatic = true;

		Particle* pTop = AddParticle(Vector2i(X, WORLD_SIZE.y - WORLD_EDGE));
		pTop->isStatic = true;
	}

	for (int Y = WORLD_EDGE; Y < WORLD_SIZE.y - WORLD_EDGE; Y += PARTICLE_COLLIDER_SIZE * 2)
	{
		Particle* pBot = AddParticle(Vector2i(WORLD_EDGE, Y));
		pBot->isStatic = true;

		Particle* pTop = AddParticle(Vector2i(WORLD_SIZE.x - WORLD_EDGE, Y));
		pTop->isStatic = true;
	}

}

void Simulation::ResetSimulationToExampleOne()
{
	ResetSimulation();
	ResetSimulation();
	ResetSimulation();
	ResetSimulation();

	int target = particleSystem->livingParticleCount + STARTING_PARTICLE_COUNT;

	for (float y = 200 + PARTICLE_COLLIDER_SIZE * 2; y < 400 + PARTICLE_COLLIDER_SIZE * 2; y += PARTICLE_COLLIDER_SIZE)
	{
		for (float x = WORLD_EDGE + (PARTICLE_COLLIDER_SIZE * 2); x <= WORLD_SIZE.x - WORLD_EDGE - (PARTICLE_COLLIDER_SIZE * 2); x += PARTICLE_COLLIDER_SIZE)
		{
			//target was STARTING_PARTICLE_COUNT
			if (particleSystem->livingParticleCount < target)
			{
				float jitter = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				Particle* p = particleSystem->GetFreshParticle();
				p->GetModel()->SetPosition(Vector2f(x + jitter, y));
				grid->Populate(p);
			}
		}
	}
}

void Simulation::ResetSimulationToExampleTwo()
{
	ResetSimulation();
	ResetSimulation();
	ResetSimulation();
	ResetSimulation();

	int radius = 100;

	Vector2f centre = Vector2f(2 * WORLD_SIZE.x / 5, 300);
	Vector2i pos;

	for (int angle = 0; angle < 360; angle += 10)
	{
		pos.x = cos((float)angle) * radius + centre.x;
		pos.y = sin((float)angle) * radius + centre.y;

		AddParticle(pos);
	}

	centre = Vector2f(3 * WORLD_SIZE.x / 5, 300);
	for (int angleTwo = 0; angleTwo < 360; angleTwo += 10)
	{
		pos.x = cos((float)angleTwo) * radius + centre.x;
		pos.y = sin((float)angleTwo) * radius + centre.y;

		AddParticle(pos);
	}

	centre = Vector2f(4 * WORLD_SIZE.x / 5, 300);
	for (int angleThree = 0; angleThree < 360; angleThree += 10)
	{
		pos.x = cos((float)angleThree) * radius + centre.x;
		pos.y = sin((float)angleThree) * radius + centre.y;

		AddParticle(pos);
	}

	for (int y = WORLD_SIZE.y - (WORLD_EDGE * 6); y < WORLD_SIZE.y - (WORLD_EDGE * 2); y += PARTICLE_COLLIDER_SIZE * 2)
	{
		for (int x = WORLD_EDGE * 2; x < WORLD_SIZE.x - (WORLD_EDGE * 2); x += 50)
		{
			Vector2i pos = Vector2i(x, y);
			AddParticle(pos);
		}
	}
}

void Simulation::ResetSimulationToExampleThree()
{
	ResetSimulation();
	ResetSimulation();
	ResetSimulation();
	ResetSimulation();
	
	for (int i = 0; i < 4; i += 2)
	{
		for (int X = i * 200; X < i * 400; X += PARTICLE_COLLIDER_SIZE * 2)
		{
			for (int Y = 200; Y < 400; Y += PARTICLE_COLLIDER_SIZE * 4)
			{
				AddParticle(Vector2i(X, Y));
			}
		}
	}	
}

void Simulation::ToggleIsRunning()
{
	isRunning = !isRunning;
}

void Simulation::IncrementRenderingDetails()
{
	if (isRenderingDetails + 1 > 3)
		isRenderingDetails = 0;
	else
		isRenderingDetails++;
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
				if (Collisions::CircleInCircle(cell->neighbours[i]->particles[j]->GetModel()->GetPosition(), PARTICLE_INTERACTION_DISTANCE, particle->GetModel()->GetPosition(), PARTICLE_INTERACTION_DISTANCE))
				{
					locals->push_back(cell->neighbours[i]->particles[j]);
				}
			}
		}

		//Check your cells particles too
		for (int k = 0; k < cell->particles.size(); k++)
		{
			if (Collisions::CircleInCircle(cell->particles[k]->GetModel()->GetPosition(), PARTICLE_INTERACTION_DISTANCE, particle->GetModel()->GetPosition(), PARTICLE_INTERACTION_DISTANCE))
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
	marchingGrid->ClearCells();

	for (int i = 0; i < particleSystem->livingParticleCount; i++)
	{
		//Populating grid with particles for this frame.
		grid->Populate(particleSystem->LivingParticles[i]);
		marchingGrid->Populate(particleSystem->LivingParticles[i]);
	}

	if (isRunning)
	{
		std::vector<Particle*> allLocalParticles;

		for (int i = 0; i < particleSystem->livingParticleCount; i++)
		{
			if (particleSystem->LivingParticles[i]->isAlive == false)
			{
				//Make the particle system aware it needs to be disposed of.
				particleSystem->KillParticle(particleSystem->LivingParticles[i]);
				continue;
			}

			//Empty the previous particles neighbours.
			allLocalParticles.clear();

			//Populate the vector with the local particles
			GetLocalParticlesFromGrid(&allLocalParticles, particleSystem->LivingParticles[i]);

			//Let the target particle know which are its neighbours so it can use them in the SPH model.
			particleSystem->LivingParticles[i]->GetModel()->LocalParticles = allLocalParticles;

			//Update target particle.
			particleSystem->LivingParticles[i]->Update(DeltaTime);

			//Perform collision detection checks on each of the neighbour particles to the target particle.
			for (int neighbours = 0; neighbours < allLocalParticles.size(); neighbours++)
			{
				if (Collisions::CircleInCircle(
					particleSystem->LivingParticles[i]->GetModel()->GetPosition(), particleSystem->LivingParticles[i]->GetColliderRadius(),
					allLocalParticles[neighbours]->GetModel()->GetPosition(), allLocalParticles[neighbours]->GetColliderRadius()))
				{

					if (particleSystem->LivingParticles[i]->isStatic == false && allLocalParticles[neighbours]->isStatic == false)
					{
						//Both non static, no collision bc physics
					}

					if (particleSystem->LivingParticles[i]->isStatic == true && allLocalParticles[neighbours]->isStatic == false)
					{
						//solve from their perspective
						allLocalParticles[neighbours]->ResolveCollision(particleSystem->LivingParticles[i]);
					}

					if (particleSystem->LivingParticles[i]->isStatic == false && allLocalParticles[neighbours]->isStatic == true)
					{
						//Solve the collision from my perspective
						particleSystem->LivingParticles[i]->ResolveCollision(allLocalParticles[neighbours]);
					}

					if (particleSystem->LivingParticles[i]->isStatic == true && allLocalParticles[neighbours]->isStatic == true)
					{
						//Both static, no collision, dont want them to display each other
					}
				}
			}
		}
	}
}

//particleSystem->LivingParticles[i]->ResolveCollision(allLocalParticles[neighbours]);

void Simulation::Render()
{
	particleSystem->SetParticleRenderDetail(isRenderingDetails);
	particleSystem->Render();

	if (particleSystem->livingParticleCount > 0)
	{
		if(isRenderingDetails != 3)
			marchingGrid->RenderMarchingSquares();
	}	

	if (isRunning == false)
	{
		Renderer::RenderText("Paused", 52.0f, Vector2f(WORLD_SIZE.x / 2 - (52.0f / 2), WORLD_SIZE.y / 2 - (52.0f / 2)), sf::Color::Red);
	}

	std::string countStr = "Particles: " + std::to_string(particleSystem->livingParticleCount) + "/" + std::to_string(particleSystem->MaxParticleCount) + '\n';
	Renderer::RenderText(countStr, 24.0f, Vector2f(52.0f, 40.0f), sf::Color::White);
}