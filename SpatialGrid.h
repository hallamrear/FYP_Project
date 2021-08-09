#pragma once
#include "LinkedList.h"
#include "Vector.h"


#include "Renderer.h"

class GridCell
{
public:
	std::vector<Particle*> particles;

	std::vector<GridCell*> neighbours;
	
	float PointWeight = 0.0f;

	GridCell()
	{
		particles = std::vector<Particle*>();
		neighbours = std::vector<GridCell*>();

		neighbours.clear();
		particles.clear();
	}

	GridCell(Vector2i ID)
	{
		particles = std::vector<Particle*>();
		neighbours = std::vector<GridCell*>();

		neighbours.clear();
		particles.clear();
	}

	void UpdatePointWeight()
	{
		if (particles.size() != 0)
			PointWeight = 1.0f;
		else
			PointWeight = 0.0f;
	}
};

class SpatialGrid
{
	sf::Image mb_img;
	sf::Sprite mb_sprite;
	int mb_size = 400 * 400 * 4;
	sf::Uint8* mb_pixels;
	sf::Texture mb_tex;

	GridCell* surrounding[9];

	UINT totalCells;
	GridCell* cells;
	Vector2f cellSize;
	Vector2i gridSize;
	void PopulateCellsWithNeighbours(Vector2i cellPos, GridCell* cellToPopulate);
	void UpdateNeighbours(int i, int j);

public:
	/// <param name="grid_size">Number of grid cells in each direction</param>
	/// <param name="cell_size">Size of each cell in world space</param>
	SpatialGrid(Vector2i grid_size, Vector2f cell_size);

	GridCell* GetCellContainingParticle(Particle* particle);

	void Populate(Particle* particle);
	void ClearCells();

	Vector2i CalculateCellPos(Vector2f worldPos);
	int CalculateArrayIDFromCellPos(Vector2i cellPos);

	void Update(float DeltaTime);

	void RenderMarchingSquares();
	int CalculateWeights(int i, int j);
	void RenderGrid();
	void RenderMetaBalls();
};

