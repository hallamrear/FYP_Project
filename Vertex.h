#pragma once
class Vertex
{
public:
	XMFLOAT3 Position;
	XMFLOAT2 TexCoords;

	Vertex();
	Vertex(XMFLOAT2 position, XMFLOAT2 texcoords);
	~Vertex();
};

