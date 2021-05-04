#include "pch.h"
#include "SpatialGrid.h"
#include "Particle.h"
#include "SFML\Graphics.hpp"
#include "Renderer.h"

void SpatialGrid::PopulateCellsWithNeighbours(Vector2i cellPos, GridCell* cellToPopulate)
{
	GridCell* cell;

	int neighbourID = 0;

	//for (int i = 0; i < 2; i++)
	//{
	//	for (int j = 0; j < 2; j++)
	//	{
	//		if (cellPos.x + i - 1 > -1 &&
	//			cellPos.x + i - 1 < gridSize.x + 1 &&
	//			cellPos.y + j - 1 > -1 &&
	//			cellPos.y + j - 1 < gridSize.y + 1
	//			)
	//		{
	//			neighbourID = CalculateArrayIDFromCellPos(Vector2i(cellPos.x + i - 1, cellPos.y + j - 1));
	//			cell = &cells[neighbourID];

	//			if (cell != nullptr)
	//			{
	//				cellToPopulate->neighbours.push_back(cell);
	//			}
	//		}
	//		else
	//			continue;
	//	}
	//}

	if (cellPos.x == 0 || cellPos.x == gridSize.x)
		return;

	if (cellPos.y == 0 || cellPos.y == gridSize.y)
		return;

	int ID = CalculateArrayIDFromCellPos(Vector2i(cellPos.x - 1, cellPos.y - 1));
	if (ID != INT_MAX)
	{
		cell = &cells[ID];
		cellToPopulate->neighbours.push_back(cell);
	}

	ID = CalculateArrayIDFromCellPos(Vector2i(cellPos.x, cellPos.y - 1));
	if (ID != INT_MAX)
	{
		cell = &cells[ID];
		cellToPopulate->neighbours.push_back(cell);
	}


	ID = CalculateArrayIDFromCellPos(Vector2i(cellPos.x + 1, cellPos.y - 1));
	if (ID != INT_MAX)
	{
		cell = &cells[ID];
		cellToPopulate->neighbours.push_back(cell);
	}

	ID = CalculateArrayIDFromCellPos(Vector2i(cellPos.x - 1, cellPos.y));
	if (ID != INT_MAX)
	{
		cell = &cells[ID];
		cellToPopulate->neighbours.push_back(cell);
	}

	ID = CalculateArrayIDFromCellPos(Vector2i(cellPos.x, cellPos.y));
	if (ID != INT_MAX)
	{
		cell = &cells[ID];
		cellToPopulate->neighbours.push_back(cell);
	}

	ID = CalculateArrayIDFromCellPos(Vector2i(cellPos.x + 1, cellPos.y));
	if (ID != INT_MAX)
	{
		cell = &cells[ID];
		cellToPopulate->neighbours.push_back(cell);
	}

	ID = CalculateArrayIDFromCellPos(Vector2i(cellPos.x - 1, cellPos.y + 1));
	if (ID != INT_MAX)
	{
		cell = &cells[ID];
		cellToPopulate->neighbours.push_back(cell);
	}

	ID = CalculateArrayIDFromCellPos(Vector2i(cellPos.x, cellPos.y + 1));
	if (ID != INT_MAX)
	{
		cell = &cells[ID];
		cellToPopulate->neighbours.push_back(cell);
	}

	ID = CalculateArrayIDFromCellPos(Vector2i(cellPos.x + 1, cellPos.y + 1));
	if (ID != INT_MAX)
	{
		cell = &cells[ID];
		cellToPopulate->neighbours.push_back(cell);
	}
}

SpatialGrid::SpatialGrid(Vector2i grid_size, Vector2f cell_size)
{
	gridSize = grid_size;
	cellSize = cell_size;
	totalCells = grid_size.x * grid_size.y;

	cells = new GridCell[totalCells];

	GridCell* cell;

	for (int y = 0; y < gridSize.y; y++)
	{
		for (int x = 0; x < gridSize.x; x++)
		{
			if (CalculateArrayIDFromCellPos(Vector2i(x, y)) != INT_MAX)
			{
				PopulateCellsWithNeighbours(Vector2i(x, y), &cells[CalculateArrayIDFromCellPos(Vector2i(x, y))]);
			}
		}
	}
}

GridCell* SpatialGrid::GetCellContainingParticle(Particle* particle)
{
	Vector2i cellPos = CalculateCellPos(particle->GetModel()->position);
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
	Vector2i cellPos = CalculateCellPos(particle->GetModel()->position);
	int cellID = CalculateArrayIDFromCellPos(cellPos);

	if (cellID != INT_MAX)
	{
		cells[cellID].particles.push_back(particle);
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

Vector2i SpatialGrid::CalculateCellPos(Vector2f worldPos)
{
	Vector2i id;
	id.x = (int)(worldPos.x / cellSize.x);
	id.y = (int)(worldPos.y / cellSize.y);
	return id;
}

int SpatialGrid::CalculateArrayIDFromCellPos(Vector2i cell_pos)
{
	//if outside the grid then disregard
	if (cell_pos.x > 0 &&
		cell_pos.x < gridSize.x &&
		cell_pos.y > 0 &&
		cell_pos.y < gridSize.y)
	{
		if ((cell_pos.y * gridSize.x) + cell_pos.x > totalCells && (cell_pos.y * gridSize.x) + cell_pos.x != INT_MAX)
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

void SpatialGrid::Update(float DeltaTime)
{

}

void SpatialGrid::RenderGrid()
{
	sf::RectangleShape shape;

	shape.setFillColor(sf::Color::Transparent);
	sf::Vector2f size = sf::Vector2f(cellSize.x, cellSize.y);
	shape.setSize(size);
	shape.setOutlineColor(sf::Color::White);
	shape.setOutlineThickness(1.0f);

	sf::Vector2f position;

	for (int y = 0; y < gridSize.y; y++)
	{
		for (int x = 0; x < gridSize.x; x++)
		{
			position.x = x * cellSize.x;
			position.y = y * cellSize.y;

			if (cells[(x * gridSize.y) + y].particles.size() == 0)
				shape.setFillColor(sf::Color::Transparent);
			else
				shape.setFillColor(sf::Color::Blue);

			shape.setPosition(position);
			Renderer::Render(&shape);
		}
	}
}

void SpatialGrid::RenderMarchingSquares()
{
	float x = 0.0f;
	float y = 0.0f;
	int draw_case = 0;
	int vertex_count = 0;

	std::vector<sf::Vertex> line_verts;

	for (int i = 0; i < gridSize.x; i++)
	{
		for (int j = 0; j < gridSize.y; j++)
		{
			vertex_count = 0;
			draw_case = CalculateWeights(i, j);

			x = i * cellSize.x;
			y = j * cellSize.y;

			sf::Vector2f a = sf::Vector2f(x + (cellSize.x * 0.5f), y);
			sf::Vector2f b = sf::Vector2f(x + (cellSize.x), y + (cellSize.y * 0.5f));
			sf::Vector2f c = sf::Vector2f(x + (cellSize.x * 0.5f), y + (cellSize.y));
			sf::Vector2f d = sf::Vector2f(x, y + (cellSize.y * 0.5f));

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				sf::RectangleShape old_shape;
				old_shape.setFillColor(sf::Color::Transparent);
				old_shape.setOutlineColor(sf::Color::Blue);
				old_shape.setOutlineThickness(0.5f);
				old_shape.setSize(sf::Vector2f(cellSize.x, cellSize.y));
				old_shape.setPosition(x, y);
				GraphicsDevice::GetWindow()->draw(old_shape);

				sf::Vertex vert = sf::Vertex(sf::Vector2f(x + (cellSize.x * 0.5f), y + (cellSize.y * 0.5f)));
				GraphicsDevice::GetWindow()->draw(&vert, 1, sf::Points);
			}

			//*--a--*
			//|	    |
			//d     b
			//|     |
			//*--c--*

			switch (draw_case)
			{
			case 0:

				break;
			case 1:
				line_verts.push_back(sf::Vertex(c));
				line_verts.push_back(sf::Vertex(d));
				vertex_count += 2;
				break;
			case 2:
				line_verts.push_back(sf::Vertex(b));
				line_verts.push_back(sf::Vertex(c));
				vertex_count += 2;
				break;
			case 3:
				line_verts.push_back(sf::Vertex(d));
				line_verts.push_back(sf::Vertex(b));
				vertex_count += 2;
				break;
			case 4:
				line_verts.push_back(sf::Vertex(a));
				line_verts.push_back(sf::Vertex(b));
				vertex_count += 2;
				break;
			case 5:
				line_verts.push_back(sf::Vertex(a));
				line_verts.push_back(sf::Vertex(d));
				line_verts.push_back(sf::Vertex(b));
				line_verts.push_back(sf::Vertex(c));
				vertex_count += 4;
				break;
			case 6:
				line_verts.push_back(sf::Vertex(a));
				line_verts.push_back(sf::Vertex(c));
				vertex_count += 2;
				break;
			case 7:
				line_verts.push_back(sf::Vertex(a));
				line_verts.push_back(sf::Vertex(d));
				vertex_count += 2;
				break;
			case 8:
				line_verts.push_back(sf::Vertex(a));
				line_verts.push_back(sf::Vertex(d));
				vertex_count += 2;
				break;
			case 9:
				line_verts.push_back(sf::Vertex(a));
				line_verts.push_back(sf::Vertex(c));
				vertex_count += 2;
				break;
			case 10:
				line_verts.push_back(sf::Vertex(d));
				line_verts.push_back(sf::Vertex(c));
				line_verts.push_back(sf::Vertex(a));
				line_verts.push_back(sf::Vertex(b));
				vertex_count += 4;
				break;
			case 11:
				line_verts.push_back(sf::Vertex(a));
				line_verts.push_back(sf::Vertex(b));
				vertex_count += 2;
				break;
			case 12:
				line_verts.push_back(sf::Vertex(d));
				line_verts.push_back(sf::Vertex(b));
				vertex_count += 2;
				break;
			case 13:
				line_verts.push_back(sf::Vertex(c));
				line_verts.push_back(sf::Vertex(b));
				vertex_count += 2;
				break;
			case 14:
				line_verts.push_back(sf::Vertex(c));
				line_verts.push_back(sf::Vertex(d));
				vertex_count += 2;
				break;
			case 15:
				break;
			default:
				break;
			}
		}
	}
	GraphicsDevice::GetWindow()->draw(line_verts.data(), line_verts.size(), sf::Lines);
}

int SpatialGrid::CalculateWeights(int i, int j)
{
	int threshold = 0;
	GridCell* cell = &cells[(j * gridSize.x) + i];

	// 0 1 2   * a *
	// 3 4 5   d   b
	// 6 7 8   * c *
	int a = 0;
	int b = 0;
	int c = 0;
	int d = 0;

	float TL = 0.0f, TR = 0.0f, BL = 0.0f, BR = 0.0f;

	//TL = cell->neighbours[0]->GetPointW + cell->neighbours[1]->PointWeight + cell->neighbours[3]->PointWeight / 3;
	//TR = cell->neighbours[1]->PointWeight + cell->neighbours[2]->PointWeight + cell->neighbours[5]->PointWeight / 3;
	//BL = cell->neighbours[3]->PointWeight + cell->neighbours[6]->PointWeight + cell->neighbours[7]->PointWeight / 3;
	//BR = cell->neighbours[5]->PointWeight + cell->neighbours[7]->PointWeight + cell->neighbours[8]->PointWeight / 3;

	if (TL > threshold)
		a = 1;
	if (TR > threshold)
		b = 1;
	if (BR > threshold)
		c = 1;
	if (BL > threshold)
		d = 1;

	return ((a * 8) + (b * 4) + (c * 2) + (d * 1));
}

