#pragma once
#include "Vector.h"

class Particle;

class TreeBoundary
{
public:
	int width = 0;
	int height = 0;
	int pos_x = 0;
	int pos_y = 0;

	Vector2f center;

	TreeBoundary()
	{

	}

	TreeBoundary(int w, int h, int x, int y)
		: width(w), height(h), pos_x(x), pos_y(y)
	{
		center.x = pos_x + (width / 2);
		center.y = pos_y + (height / 2);
	}

	bool Contains(Vector2f position)
	{
		//Edge collision detection
		if (position.x < pos_x + width && position.x > pos_x - width &&
			position.y < pos_y + height && position.y > pos_y - height)
			return true;
		else
			return false;
	};

	bool Contains(TreeBoundary boundary)
	{
		return (abs((pos_x + width / 2) - (boundary.pos_x + boundary.width / 2)) * 2 < (width + boundary.width)) &&
			(abs((pos_y + height / 2) - (boundary.pos_y + boundary.height / 2)) * 2 < (height + boundary.height));
	};
};

class Quadtree
{
public:
	TreeBoundary* boundary;
	bool isDivided;
	int depth;
	int capacity;

	Quadtree* parent;
	Quadtree* topLeft;
	Quadtree* topRight;
	Quadtree* bottomLeft;
	Quadtree* bottomRight;

	std::vector<Particle*> data;

	Quadtree(TreeBoundary* tree_boundary, Quadtree* tree_parent, int tree_depth, int tree_capacity);
	~Quadtree();

	void Insert(Particle* particle);
	void Remove(Particle* particle);
	std::vector<Particle*> Retrieve(std::vector<Particle*> particles_retrieved, TreeBoundary boundary);
	void Subdivide();

	void Update(float DeltaTime);
	void Render();
};

