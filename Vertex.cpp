#include "pch.h"
#include "Vertex.h"

Vertex::Vertex()
{
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	TexCoords = XMFLOAT2(0.0f, 0.0f);
}

Vertex::Vertex(XMFLOAT2 position, XMFLOAT2 texcoords)
{
	Position = XMFLOAT3(position.x, position.y, 0.0f);
	TexCoords = texcoords;
}

Vertex::~Vertex()
{
}
