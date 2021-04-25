#include "pch.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "Renderer.h"
#include "Vertex.h"

#include "NewRenderer.h"


ParticleSystem::ParticleSystem(int MaxParticleCount)
{

    deadParticleCount = 0;
    livingParticleCount = 0;

    this->MaxParticleCount = MaxParticleCount;

    Particle* particle;

    for (int i = 0; i < MaxParticleCount; i++)
    {
        particle = new Particle();
        particle->poolID = i;
        deadParticleCount++;
        deadParticlesList = LinkedList::PushFront(deadParticlesList, particle);
        ParticlePool.push_back(particle);
    }

    particle = nullptr;

    CreateBuffers();
}

ParticleSystem::~ParticleSystem()
{

}

HRESULT ParticleSystem::CreateBuffers()
{
	HRESULT hr = E_FAIL;

	//TODO : GET RID
	// Set the number of instances in the array.
	int instanceCount = MaxParticleCount;
	XMFLOAT3* instances = new XMFLOAT3[instanceCount];
	// Load the instance array with data.
	//THIS IS THE POSITIONS OF THE INSTANCES NOT THE VERTEX DATA
	for(int i = 0; i < instanceCount; i++)
		instances[i] = XMFLOAT3(-0.0F, 0.0F, 0.0F);

	///Creating Instance buffer  
	D3D11_BUFFER_DESC instanceDesc;
	ZeroMemory(&instanceDesc, sizeof(instanceDesc));
	instanceDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	instanceDesc.ByteWidth = sizeof(XMFLOAT3) * instanceCount;
	instanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceDesc.MiscFlags = 0;
	instanceDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA instanceData;
	ZeroMemory(&instanceData, sizeof(instanceData));
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	hr = GraphicsDevice::GetDevice()->CreateBuffer(&instanceDesc, &instanceData, &InstanceBuffer);
	if (FAILED(hr))
		return hr;

	// Set the number of vertices in the vertex array.
	int vertexCount = 6;
	// Create the vertex array.
	Vertex* vertices = new Vertex[vertexCount];

	//We want a tiny square
	vertices[0].Position = XMFLOAT3(-2.0f, -2.0f, 0.0f);  // Bottom left.
	vertices[0].TexCoords = XMFLOAT2(0.0f, 1.0f);

	vertices[1].Position = XMFLOAT3(-2.0f, 2.0f, 0.0f);  // Top left.
	vertices[1].TexCoords = XMFLOAT2(0.0f, 0.0f);

	vertices[2].Position = XMFLOAT3(2.0f, 2.0f, 0.0f);  // top right.
	vertices[2].TexCoords = XMFLOAT2(1.0f, 0.0f);

	vertices[3].Position = XMFLOAT3(-2.0f, -2.0f, 0.0f);  // Bottom left.
	vertices[3].TexCoords = XMFLOAT2(0.0f, 1.0f);

	vertices[4].Position = XMFLOAT3(2.0f, 2.0f, 0.0f);  // top right.
	vertices[4].TexCoords = XMFLOAT2(1.0f, 0.0f);

	vertices[5].Position = XMFLOAT3(2.0f, -2.0f, 0.0f);  // Bottom right.
	vertices[5].TexCoords = XMFLOAT2(1.0f, 1.0f);

	//Create a vertex Buffer
	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(vertexDesc));
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.CPUAccessFlags = 0;
	vertexDesc.MiscFlags = 0;
	vertexDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hr = GraphicsDevice::GetDevice()->CreateBuffer(&vertexDesc, &vertexData, &VertexBuffer);
	if (FAILED(hr))
		return hr;

	// BIN IT OFF
	delete[] instances;
	instances = 0;
	delete[] vertices;
	vertices = 0;


	return S_OK;
}

void ParticleSystem::UpdateInstanceBuffer()
{
	//TODO : Implement

	/*
	FROM MSDN  https://docs.microsoft.com/en-us/windows/win32/direct3d11/how-to--use-dynamic-resources
	when you call Map with D3D11_MAP_WRITE_DISCARD, the runtime
	returns a pointer to a new region of memory	instead of
	the old buffer data. This allows the GPU to continue
	using the old data while the app places data in the new buffer
	*/

	std::vector<XMFLOAT3> instancePositions;

	for (int i = 0; i < livingParticleCount; i++)
	{
		instancePositions.push_back(XMFLOAT3(
			livingParticles[i]->model.position.x,
			livingParticles[i]->model.position.y,
			0.0f));
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//Stop graphics card accessing 
	GraphicsDevice::GetContext()->Map(InstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	//Change data
	memcpy(mappedResource.pData, instancePositions.data(), sizeof(XMFLOAT3) * instancePositions.size());

	//Give it back to the GPU
	GraphicsDevice::GetContext()->Unmap(InstanceBuffer, 0);
}

Particle* ParticleSystem::GetFreshParticle()
{
    if (deadParticleCount > 0)
    {
        Particle* particle = deadParticlesList->data;
		particle->CreateRandom();
        deadParticlesList = LinkedList::DeleteFront(deadParticlesList);
		livingParticles.push_back(particle);
        livingParticleCount++;
        deadParticleCount--;
        return particle;
    }

    return nullptr;
}

void ParticleSystem::KillParticle(Particle* particle)   
{
    if (livingParticleCount > 0)
    {
        particle->Kill();
        deadParticlesList = LinkedList::PushFront(deadParticlesList, particle);
        livingParticleCount--;
        deadParticleCount++;
    }
}

void ParticleSystem::Update(float DeltaTime)
{
	UpdateInstanceBuffer();
}

void ParticleSystem::Render()
{
	for(int i = 0; i < livingParticleCount; i++)
		ParticleRenderer::Render(livingParticles[i]);


//    Renderer::Get()->PrepareGeometryRender();
//    
//	//Stride of vertex, stride of instance data
//	unsigned int strideArray[2] = { sizeof(Vertex), sizeof(XMFLOAT3) };
//	//Both 0
//	unsigned int offsetArray[2] = { 0 };
//	ID3D11Buffer* bufferArray[2] = { VertexBuffer, InstanceBuffer };
//
//	//Let directX know we're using multiple buffers
//	GraphicsDevice::GetContext()->IASetVertexBuffers(0, 2, bufferArray, strideArray, offsetArray);
//
//    GraphicsDevice::GetContext()->DrawInstanced(6, MaxParticleCount, 0, 0);
}
