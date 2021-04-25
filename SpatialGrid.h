#pragma once
#include "LinkedList.h"

class GridCell
{
public:
	std::vector<Particle*> particles;
	std::vector<GridCell*> neighbours;

	GridCell()
	{

	}

	GridCell(XMINT2 ID)
	{

	}
};

class SpatialGrid
{
	UINT totalCells;
	GridCell* cells;
	XMFLOAT2 cellSize;
	XMINT2 gridSize;
	void PopulateCellsWithNeighbours(XMINT2 cellPos, GridCell* cellToPopulate);

public:
	/// <param name="grid_size">Number of grid cells in each direction</param>
	/// <param name="cell_size">Size of each cell in world space</param>
	SpatialGrid(XMINT2 grid_size, XMFLOAT2 cell_size);

	GridCell* GetCellContainingParticle(Particle* particle);

	void Populate(Particle* particle);
	void ClearCells();

	XMINT2 CalculateCellPos(XMFLOAT2 worldPos);
	int CalculateArrayIDFromCellPos(XMINT2 cell_pos);
};

