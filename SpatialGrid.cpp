#include "pch.h"
#include "SpatialGrid.h"
#include "Particle.h"

void SpatialGrid::PopulateCellsWithNeighbours(XMINT2 cellPos,	 GridCell* cellToPopulate)
{
	GridCell* cell;

	int neighbourID = 0;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			if( cellPos.x + i - 1 > -1 &&
				cellPos.x + i - 1 < gridSize.x + 1 &&
				cellPos.y + j - 1 > -1 &&
				cellPos.y + j - 1 < gridSize.y + 1
				)
			{
				neighbourID = CalculateArrayIDFromCellPos(XMINT2(cellPos.x + i - 1, cellPos.y + j - 1));
				cell = &cells[neighbourID];

				if (cell != nullptr)
				{
					cellToPopulate->neighbours.push_back(cell);
				}

			}
			else
			{
				continue;
			}
		}
	}


}

SpatialGrid::SpatialGrid(XMINT2 grid_size, XMFLOAT2 cell_size)
{
	gridSize = grid_size;
	cellSize = cell_size;
	totalCells = grid_size.x * grid_size.y;

	cells = new GridCell[totalCells];
}

GridCell* SpatialGrid::GetCellContainingParticle(Particle* particle)
{
	XMINT2 cellPos = CalculateCellPos(particle->GetModel()->position);
	int cellID = CalculateArrayIDFromCellPos(cellPos);

	if (cellID != INT_MAX)
	{
		return &cells[cellID];
	}
	else
		return nullptr;
}

void SpatialGrid::Populate(Particle* particle)
{
	XMINT2 cellPos = CalculateCellPos(particle->GetModel()->position);
	int cellID = CalculateArrayIDFromCellPos(cellPos);

	if (cellID != INT_MAX)
	{
		cells[cellID].particles.push_back(particle);
		PopulateCellsWithNeighbours(cellPos, &cells[cellID]);
	}	
}

void SpatialGrid::ClearCells()
{
	for (int i = 0; i < totalCells; i++)
	{
		cells[i].particles.clear();
		cells[i].neighbours.clear();
	}
}

XMINT2 SpatialGrid::CalculateCellPos(XMFLOAT2 worldPos)
{
	XMINT2 id;
	id.x = (int)(worldPos.x / cellSize.x);
	id.y = (int)(worldPos.y / cellSize.y);
	return id;
}

int SpatialGrid::CalculateArrayIDFromCellPos(XMINT2 cell_pos)
{
	//if outside the grid then disregard
	if (cell_pos.x > 0 &&
		cell_pos.x < gridSize.x &&
		cell_pos.y > 0 &&
		cell_pos.y < gridSize.y)
	{
		if ((cell_pos.x * gridSize.y) + cell_pos.y > 10000 && (cell_pos.x * gridSize.y) + cell_pos.y != INT_MAX)
		{
			//IF STILL BROKE RETURN IT TO (cell_pos.x * gridSize.y) + cell_pos.y
			OutputDebugStringA("asdasd");
			return INT_MAX;
		}

		return (int)((cell_pos.x * gridSize.y) + cell_pos.y);
	}
	else
		return INT_MAX;
}
