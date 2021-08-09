#include "pch.h"
#include "SpatialGrid.h"
#include "Particle.h"
#include "SFML\Graphics.hpp"
#include "Overloads.h"
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

	if (cellPos.x < 0 || cellPos.x > gridSize.x)
	{
		OutputDebugStringA("");
		return;
	}

	if (cellPos.y < 0 || cellPos.y > gridSize.y)
	{
		OutputDebugStringA("");
		return;
	}

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

	mb_size = WORLD_SIZE.x * WORLD_SIZE.y * 4;
	mb_pixels = new sf::Uint8[mb_size];

	mb_img.loadFromMemory(mb_pixels, mb_size);
	mb_tex.loadFromImage(mb_img);
	mb_sprite.setTexture(mb_tex);

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
	Vector2i cellPos = CalculateCellPos(particle->GetModel()->GetPosition());
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
	Vector2i cellPos = CalculateCellPos(particle->GetModel()->GetPosition());
	int cellID = CalculateArrayIDFromCellPos(cellPos);

	if (cellID != INT_MAX)
	{
		cells[cellID].particles.push_back(particle);
		cells[cellID].UpdatePointWeight();
	}	
}

void SpatialGrid::ClearCells()
{
	for (int i = 0; i < totalCells; i++)
	{
		cells[i].particles.clear();
		cells[i].UpdatePointWeight();
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
	if (cell_pos.x >= 0 &&
		cell_pos.x < gridSize.x &&
		cell_pos.y >= 0 &&
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

			shape.setFillColor(sf::Color::Transparent);

			if (cells[(x * gridSize.y) + y].particles.size() > 0)
				shape.setFillColor(sf::Color::Blue);

			shape.setPosition(position);
			Renderer::RenderShape(&shape);
		}
	}
}

void SpatialGrid::RenderMarchingSquares()
{
	float x = 0.0f;
	float y = 0.0f;
	int draw_case = 0;

	for (int i = 0; i < gridSize.x; i++)
	{
		for (int j = 0; j < gridSize.y; j++)
		{
			draw_case = CalculateWeights(i, j);

			x = i * cellSize.x;
			y = j * cellSize.y;

			sf::Vector2f a = sf::Vector2f(x + (cellSize.x * 0.5f), y);
			sf::Vector2f b = sf::Vector2f(x + (cellSize.x)	     , y + (cellSize.y * 0.5f));
			sf::Vector2f c = sf::Vector2f(x + (cellSize.x * 0.5f), y + (cellSize.y));
			sf::Vector2f d = sf::Vector2f(x						 , y + (cellSize.y * 0.5f));

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				sf::RectangleShape old_shape;
				old_shape.setFillColor(sf::Color::Transparent);
				old_shape.setOutlineColor(sf::Color::Blue);
				old_shape.setOutlineThickness(0.5f);
				old_shape.setSize(sf::Vector2f(cellSize.x, cellSize.y));
				old_shape.setPosition(x, y);
				GraphicsDevice::GetWindow()->draw(old_shape);

				if(draw_case != 0)
					Renderer::RenderText(std::to_string(draw_case), 16.0, Vector2f(x, y), sf::Color::Red);
			}

			//*--a--*
			//|	    |
			//d     b
			//|     |
			//*--c--*
			sf::RectangleShape shape;

			switch (draw_case)
			{
			case 0:

				break;
			case 1:
				Renderer::RenderLine(Vector2f(c.x, c.y), Vector2f(d.x, d.y), 1.0f, sf::Color::White);
				break;
			case 2:

				Renderer::RenderLine(Vector2f(b.x, b.y), Vector2f(c.x, c.y), 1.0f, sf::Color::White);
				break;
			case 3:
				//
				Renderer::RenderLine(Vector2f(d.x, d.y), Vector2f(b.x, b.y), 1.0f, sf::Color::White);
				break;
			case 4:

				Renderer::RenderLine(Vector2f(a.x, a.y), Vector2f(b.x, b.y), 1.0f, sf::Color::White);
				break;
			case 5:
				Renderer::RenderLine(Vector2f(a.x, a.y), Vector2f(d.x, d.y), 1.0f, sf::Color::White);
				Renderer::RenderLine(Vector2f(b.x, b.y), Vector2f(c.x, c.y), 1.0f, sf::Color::White);

				break;
			case 6:
				//
				Renderer::RenderLine(Vector2f(a.x, a.y), Vector2f(c.x, c.y), 1.0f, sf::Color::White);
				break;
			case 7:
				Renderer::RenderLine(Vector2f(a.x, a.y), Vector2f(d.x, d.y), 1.0f, sf::Color::White);
				break;
			case 8:
				Renderer::RenderLine(Vector2f(a.x, a.y), Vector2f(d.x, d.y), 1.0f, sf::Color::White);
				break;
			case 9:
				//
				Renderer::RenderLine(Vector2f(a.x, a.y), Vector2f(c.x, c.y), 1.0f, sf::Color::White);
				break;
			case 10:
				Renderer::RenderLine(Vector2f(d.x, d.y), Vector2f(c.x, c.y), 1.0f, sf::Color::White);
				Renderer::RenderLine(Vector2f(a.x, a.y), Vector2f(b.x, b.y), 1.0f, sf::Color::White);
				break;
			case 11:
				Renderer::RenderLine(Vector2f(a.x, a.y), Vector2f(b.x, b.y), 1.0f, sf::Color::White);
				break;
				Renderer::RenderLine(Vector2f(d.x, d.y), Vector2f(b.x, b.y), 1.0f, sf::Color::White);
				break;
			case 12:
				//
				Renderer::RenderLine(Vector2f(d.x, d.y), Vector2f(b.x, b.y), 1.0f, sf::Color::White);
				break;
			case 13:
				//
				Renderer::RenderLine(Vector2f(c.x, c.y), Vector2f(b.x, b.y), 1.0f, sf::Color::White);
				break;
			case 14:
				Renderer::RenderLine(Vector2f(c.x, c.y), Vector2f(d.x, d.y), 1.0f, sf::Color::White);
				break;
			case 15:
				break;
			default:
				break;
			}
		}
	}
}

int SpatialGrid::CalculateWeights(int i, int j)
{
	float threshold = 0.0f;
	GridCell* cell = &cells[(j * gridSize.x) + i];

	UpdateNeighbours(i, j);

	// 0 1 2   * a *
	// 3 4 5   d   b
	// 6 7 8   * c *
	int a = 0;
	int b = 0;
	int c = 0;
	int d = 0;	

	float TL = 0.0f, TR = 0.0f, BL = 0.0f, BR = 0.0f;

	if (cell->neighbours.size() != 0)
	{
		if (surrounding[0])
			if(isnan(surrounding[0]->PointWeight) || surrounding[0]->PointWeight != FLT_MAX)
				TL += surrounding[0]->PointWeight;
		if (surrounding[1])
			if (isnan(surrounding[1]->PointWeight) || surrounding[1]->PointWeight != FLT_MAX)
			TL += surrounding[1]->PointWeight;
		if (surrounding[3])
			if (isnan(surrounding[3]->PointWeight) || surrounding[3]->PointWeight != FLT_MAX)
			TL += surrounding[3]->PointWeight;
		if (TL != 0.0f)
			TL /= 3;

		if (surrounding[1])
			if (isnan(surrounding[1]->PointWeight) || surrounding[1]->PointWeight != FLT_MAX)
				TR += surrounding[1]->PointWeight;
		if (surrounding[2])
			if (isnan(surrounding[2]->PointWeight) || surrounding[2]->PointWeight != FLT_MAX)
				TR += surrounding[2]->PointWeight;
		if (surrounding[5])
			if (isnan(surrounding[5]->PointWeight) || surrounding[5]->PointWeight != FLT_MAX)
				TR += surrounding[5]->PointWeight;
		if (TR != 0.0f)
			TR /= 3;

		if (surrounding[3])
			if (isnan(surrounding[3]->PointWeight) || surrounding[3]->PointWeight != FLT_MAX)
				BL += surrounding[3]->PointWeight;
		if (surrounding[6])
			if (isnan(surrounding[6]->PointWeight) || surrounding[6]->PointWeight != FLT_MAX)
				BL += surrounding[6]->PointWeight;
		if (surrounding[7])
			if (isnan(surrounding[7]->PointWeight) || surrounding[7]->PointWeight != FLT_MAX)
				BL += surrounding[7]->PointWeight;
		if (BL != 0.0f)
			BL /= 3;

		if (surrounding[5])
			if (isnan(surrounding[5]->PointWeight) || surrounding[5]->PointWeight != FLT_MAX)
				BR += surrounding[5]->PointWeight;
		if (surrounding[7])
			if (isnan(surrounding[7]->PointWeight) || surrounding[7]->PointWeight != FLT_MAX)
				BR += surrounding[7]->PointWeight;
		if (surrounding[8])
			if (isnan(surrounding[8]->PointWeight) || surrounding[8]->PointWeight != FLT_MAX)
				BR += surrounding[8]->PointWeight;
		if (BR != 0.0f)
			BR /= 3;
	}

	if (TL > threshold) a = 1;
	if (TR > threshold) b = 1;
	if (BR > threshold) c = 1;
	if (BL > threshold) d = 1;

	return (a * 8) + (b * 4) + (c * 2) + d;
}

void SpatialGrid::UpdateNeighbours(int center_pos_x, int center_pos_y)
{
	if (center_pos_x - 1 >= 0 && center_pos_y - 1 >= 0)
		surrounding[0] = &cells[CalculateArrayIDFromCellPos(Vector2i(center_pos_x - 1, center_pos_y - 1))];
	
	if (center_pos_y - 1 >= 0)
		surrounding[1] = &cells[CalculateArrayIDFromCellPos(Vector2i(center_pos_x, center_pos_y - 1))];
	if (center_pos_y - 1 >= 0)
		surrounding[2] = &cells[CalculateArrayIDFromCellPos(Vector2i(center_pos_x + 1,center_pos_y - 1))];
	if (center_pos_x - 1 >= 0)
		surrounding[3] = &cells[CalculateArrayIDFromCellPos(Vector2i(center_pos_x - 1,center_pos_y))];

	surrounding[4] = &cells[CalculateArrayIDFromCellPos(Vector2i(center_pos_x,center_pos_y))];
	surrounding[5] = &cells[CalculateArrayIDFromCellPos(Vector2i(center_pos_x + 1,center_pos_y))];

	if (center_pos_x - 1 >= 0)
		surrounding[6] = &cells[CalculateArrayIDFromCellPos(Vector2i(center_pos_x - 1,center_pos_y + 1))];

	surrounding[7] = &cells[CalculateArrayIDFromCellPos(Vector2i(center_pos_x,center_pos_y + 1))];
	surrounding[8] = &cells[CalculateArrayIDFromCellPos(Vector2i(center_pos_x + 1,center_pos_y + 1))];

	for (int i = 0; i < 9; i++)
	{
		if(surrounding[i])
			surrounding[i]->UpdatePointWeight();
	}
}