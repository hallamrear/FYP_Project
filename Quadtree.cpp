#include "pch.h"
#include "Quadtree.h"
#include "Particle.h"
#include "Renderer.h"

Quadtree::Quadtree(TreeBoundary* tree_boundary, Quadtree* tree_parent, int tree_depth, int tree_capacity)
{
	capacity = tree_capacity;
	topLeft = nullptr;
	topRight = nullptr;
	bottomLeft = nullptr;
	bottomRight = nullptr;
	boundary = tree_boundary;
	parent = tree_parent;
	depth = tree_depth;
	isDivided = false;
}

Quadtree::~Quadtree()
{
	data.clear();

	if (isDivided)
	{
		delete bottomLeft;
		bottomLeft = nullptr;
		delete bottomRight;
		bottomRight = nullptr;
		delete topLeft;
		topLeft = nullptr;
		delete topRight;
		topRight = nullptr;
	}
}

void Quadtree::Insert(Particle* particle)
{
	if(!boundary->Contains(particle->GetModel()->position))
		return;

	if (data.size() != capacity)
	{
		data.push_back(particle);
	}
	else 
	{
		if (isDivided == false)
			Subdivide();

		this->topRight->Insert(particle);
		this->topLeft->Insert(particle);
		this->bottomRight->Insert(particle);
		this->bottomLeft->Insert(particle);
	}
}

void Quadtree::Remove(Particle* particle)
{

}

void Quadtree::Subdivide()
{
	isDivided = true;

	this->topRight = new Quadtree(new TreeBoundary(boundary->width / 2, boundary->height / 2, boundary->pos_x + boundary->width / 2, boundary->pos_y - boundary->height / 2), this, depth + 1, capacity);
	this->topLeft = new	Quadtree(new TreeBoundary(boundary->width / 2, boundary->height / 2, boundary->pos_x - boundary->width / 2, boundary->pos_y - boundary->height / 2), this, depth + 1, capacity);
	this->bottomRight = new	Quadtree(new TreeBoundary(boundary->width / 2, boundary->height / 2, boundary->pos_x + boundary->width / 2, boundary->pos_y + boundary->height / 2), this, depth + 1, capacity);
	this->bottomLeft = new Quadtree(new TreeBoundary(boundary->width / 2, boundary->height / 2, boundary->pos_x - boundary->width / 2, boundary->pos_y + boundary->height / 2), this, depth + 1, capacity);
}

std::vector<Particle*> Quadtree::Retrieve(std::vector<Particle*> particles_retrieved, TreeBoundary boundary)
{
	//TODO : IMPL?

	return particles_retrieved;
}

void Quadtree::Update(float DeltaTime)
{

}

void Quadtree::Render()
{
	if (isDivided)
	{
		this->topRight->Render();
		this->topLeft->Render();
		this->bottomRight->Render();
		this->bottomLeft->Render();
	}
}