#include "PCH.h"
#include "Renderer.h"
#include "Particle.h"
#include "Overloads.h"
#include <fstream>

Renderer* Renderer::instance;

static std::vector<float> test = std::vector<float>(10, 10.0f);

Renderer::Renderer()
{
    m_ParticleImage = nullptr;
    m_GPUDevice = nullptr;
    m_Renderer = nullptr;
    WindowHeight = 0;
    WindowWidth = 0;
    isInitialised = false;
    m_ParticleRectListBack = 0;
    m_ParticleRects = std::vector<SDL_FRect>();
    m_ParticleRects.resize(MAX_PARTICLE_COUNT);
    memset(&m_ParticleRects[0], 0, sizeof(SDL_FRect) * MAX_PARTICLE_COUNT);
    font = nullptr;
}

Renderer::~Renderer()
{
    
}

void Renderer::ComputePass()
{
    m_CommandBuffer = SDL_AcquireGPUCommandBuffer(m_GPUDevice);

    float* transferData = (float*)SDL_MapGPUTransferBuffer(m_GPUDevice, m_DataTransferBuffer, true);
    memcpy(&test[0], transferData, sizeof(float) * test.size());
    SDL_UnmapGPUTransferBuffer(m_GPUDevice, m_DataTransferBuffer);

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(m_CommandBuffer);

    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = m_DataTransferBuffer;
    location.offset = 0;

    SDL_GPUBufferRegion region{};
    region.buffer = m_GPUStorageBuffer;
    region.size = sizeof(float) * test.size();
    region.offset = 0;

    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

    SDL_EndGPUCopyPass(copyPass);

    SDL_GPUColorTargetInfo clearInfo = {};
    clearInfo.texture = m_GPUTexture;
    clearInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    clearInfo.store_op = SDL_GPU_STOREOP_STORE;
    clearInfo.clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };

    SDL_GPURenderPass* clearPass = SDL_BeginGPURenderPass(m_CommandBuffer, &clearInfo, 1, nullptr);
    SDL_EndGPURenderPass(clearPass);

    SDL_GPUStorageTextureReadWriteBinding texBinding = {};
    texBinding.texture = m_GPUTexture;

    SDL_GPUStorageBufferReadWriteBinding storageBinding{};
    storageBinding.buffer = m_GPUStorageBuffer;
    storageBinding.cycle = true;

    m_CurrentComputePass = SDL_BeginGPUComputePass(m_CommandBuffer, &texBinding, 1, &storageBinding, 1);
    SDL_BindGPUComputePipeline(m_CurrentComputePass, m_ComputePipeline);

    SDL_GPUBuffer* readonlyStorageBindings[] =
    {
        m_GPUConstantBuffer
    };

    SDL_BindGPUComputeStorageBuffers(m_CurrentComputePass, 0, readonlyStorageBindings, _countof(readonlyStorageBindings));
    SDL_PushGPUComputeUniformData(m_CommandBuffer, 0, &ConstantBuffer, sizeof(ConstantBuffer));

    int x = (WORLD_SIZE.x + 31) / 32;
    int y = (WORLD_SIZE.y + 31) / 32;
    SDL_DispatchGPUCompute(m_CurrentComputePass, x, y, 1);

    SDL_EndGPUComputePass(m_CurrentComputePass);
    m_CurrentComputePass = nullptr;

    //copyPass = SDL_BeginGPUCopyPass(m_CommandBuffer);
    //SDL_DownloadFromGPUBuffer(copyPass, &region, &location);
    //SDL_EndGPUCopyPass(copyPass);

    Uint32 w = WORLD_SIZE.x;
    Uint32 h = WORLD_SIZE.y;

    if (SDL_WaitAndAcquireGPUSwapchainTexture(m_CommandBuffer, GraphicsDevice::GetWindow(), &m_SwapchainTexture, &w, &h))
    {
        SDL_GPUBlitInfo blit = {};
        blit.source.texture = m_GPUTexture;
        blit.source.w = WORLD_SIZE.x;
        blit.source.h = WORLD_SIZE.y;
        blit.destination.texture = m_SwapchainTexture;
        blit.destination.w = w;
        blit.destination.h = h;
        blit.filter = SDL_GPU_FILTER_NEAREST;
        SDL_BlitGPUTexture(m_CommandBuffer, &blit);
    }

    SDL_SubmitGPUCommandBuffer(m_CommandBuffer);

}

HRESULT Renderer::Init(int width, int height)
{
    HRESULT hr = E_FAIL;

    WindowWidth = width;
    WindowHeight = height;

    if (GraphicsDevice::Get()->GetIsInitialised() == false)
    {
        GraphicsDevice::Get()->Init(width, height);
    }

    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_DXIL;

    bool debug = false;
    #ifdef  _DEBUG
    debug = true;
    #endif //  _DEBUG

    m_GPUDevice = SDL_CreateGPUDevice(format, debug, "direct3d12");
    SDL_ClaimWindowForGPUDevice(m_GPUDevice, GraphicsDevice::GetWindow());
    m_CommandBuffer = SDL_AcquireGPUCommandBuffer(m_GPUDevice);

    SDL_GPUComputePipelineCreateInfo createInfo{};
    memset(&createInfo, 0, sizeof(SDL_GPUComputePipelineCreateInfo));

    char* bytes = nullptr;
    m_ShaderByteCode = (char*)SDL_LoadFile("ComputeShader.cso", &m_ShaderByteCodeSize);

    if (m_ShaderByteCode == nullptr)
    {
        printf("%s\n", SDL_GetError());
        return E_FAIL;
    }

    createInfo.code = (Uint8*)m_ShaderByteCode;
    createInfo.code_size = m_ShaderByteCodeSize;
    createInfo.entrypoint = "main";
    createInfo.format = format;
    createInfo.threadcount_x = 32;
    createInfo.threadcount_y = 32;
    createInfo.threadcount_z = 1;
    createInfo.num_readwrite_storage_buffers = 1;
    createInfo.num_readwrite_storage_textures = 1;
    createInfo.num_readonly_storage_textures = 0;
    createInfo.num_readonly_storage_buffers = 0;
    createInfo.num_uniform_buffers = 1;

    m_ComputePipeline = SDL_CreateGPUComputePipeline(m_GPUDevice, &createInfo);

    SDL_GPUBufferCreateInfo bufferInfo{ 0 };
    bufferInfo.size = sizeof(float) * test.size();
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_WRITE | SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_READ;
    m_GPUStorageBuffer = SDL_CreateGPUBuffer(m_GPUDevice, &bufferInfo);

    SDL_GPUTransferBufferCreateInfo transferBufferInfo = {};
    transferBufferInfo.size = bufferInfo.size;
    transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    m_DataTransferBuffer = SDL_CreateGPUTransferBuffer(m_GPUDevice, &transferBufferInfo);

    SDL_GPUBufferCreateInfo cbInfo{};
    cbInfo.size = sizeof(ConstantBuffer);
    cbInfo.usage = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_READ;
    m_GPUConstantBuffer = SDL_CreateGPUBuffer(m_GPUDevice, &cbInfo);

    SDL_GPUTransferBufferCreateInfo cbTransferBufferInfo = {};
    cbTransferBufferInfo.size = cbInfo.size;
    cbTransferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    m_ConstantBufferTransferBuffer = SDL_CreateGPUTransferBuffer(m_GPUDevice, &cbTransferBufferInfo);

    SDL_GPUTextureCreateInfo textureCreateInfo{};
    memset(&textureCreateInfo, 0, sizeof(SDL_GPUTextureCreateInfo));
    textureCreateInfo.format = SDL_GPUTextureFormat::SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureCreateInfo.height = WORLD_SIZE.y;
    textureCreateInfo.width = WORLD_SIZE.x;
    textureCreateInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureCreateInfo.layer_count_or_depth = 1;
    textureCreateInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    textureCreateInfo.num_levels = 1;

    SDL_SetFloatProperty(textureCreateInfo.props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_R_FLOAT, 0.0f);
    SDL_SetFloatProperty(textureCreateInfo.props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_G_FLOAT, 0.0f);
    SDL_SetFloatProperty(textureCreateInfo.props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_B_FLOAT, 0.0f);
    SDL_SetFloatProperty(textureCreateInfo.props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_A_FLOAT, 0.0f);

    textureCreateInfo.usage = SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_WRITE | SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
    m_GPUTexture = SDL_CreateGPUTexture(m_GPUDevice, &textureCreateInfo);

    hr = S_OK;

    return hr;
}

Renderer* Renderer::Get()
{
    if (instance == nullptr)
        instance = new Renderer();

    return instance;
}

void Renderer::PrepareFrame()
{
    SDL_SetRenderDrawColorFloat(m_Renderer, 0.0f, 0.0f, 0.0f, 1.0f);
    SDL_RenderClear(m_Renderer);
}

void Renderer::PresentFrame()
{
    SDL_SetRenderDrawColorFloat(m_Renderer, 1.0F, 0.0f, 0.0f, 1.0f);
    SDL_RenderRects(m_Renderer, &m_ParticleRects[0], m_ParticleRectListBack);
    SDL_RenderPresent(m_Renderer);

    memset(&m_ParticleRects[0], 0, sizeof(SDL_FRect) * MAX_PARTICLE_COUNT);
    m_ParticleRectListBack = 0;
}

void Renderer::RenderShape_Impl(Shape* shape)
{
    //GraphicsDevice::GetWindow()->draw(*shape);
}

void Renderer::RenderVector_Impl(Vector2f position, Vector2f direction, float length)
{
    Vector2f end = { };
    end.x = position.x + (direction.x * length);
    end.y = position.y + (direction.y * length);
    SDL_SetRenderDrawColorFloat(m_Renderer, 1.0f, 0.0f, 1.0f, 1.0f);
    SDL_RenderLine(m_Renderer, position.x, position.y, end.x, end.y);
}

void Renderer::RenderLine_Impl(Vector2f start, Vector2f end, float thickness, SDL_Color color)
{
    SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, 255);
    SDL_RenderLine(m_Renderer, start.x, start.y, end.x, end.y);
}

void Renderer::RenderText_Impl(std::string str, float size, Vector2f pos, SDL_Color color)
{
    SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, 255);
   /* sf::Text text;
    text.setFont(font);
    text.setPosition(pos.x, pos.y);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    GraphicsDevice::GetWindow()->draw(text);*/
    OutputDebugString(L"RenderText_Impl not implemented.\n");
}

void Renderer::RenderParticle_Impl(Particle* particle)
{
    //TODO : Draw Circles instead.

    float r = 3.0f;

    SDL_FRect rect{};
    m_ParticleRects[m_ParticleRectListBack].x = particle->GetModel()->GetPosition().x - r;
    m_ParticleRects[m_ParticleRectListBack].y = particle->GetModel()->GetPosition().y - r;
    m_ParticleRects[m_ParticleRectListBack].w = r + r;
    m_ParticleRects[m_ParticleRectListBack].h = r + r;

    if (particle->isStatic)
        SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
    else
        SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 255);

    m_ParticleRectListBack++;

    //SDL_RenderRect(m_Renderer, &rect);
}

void Renderer::RenderParticleDetailed_Impl(Particle* particle)
{
    RenderParticle_Impl(particle);

    Vector2f dir = Vector2f(particle->GetModel()->GetVelocity());
    dir.GetNormalized();
    RenderLine(particle->GetModel()->GetPosition(), particle->GetModel()->GetPosition() + dir, 2.0f, {255, 255, 0, 255});

    SDL_Rect rect{};
    rect.x = particle->GetModel()->GetPosition().x - 1.0f;
    rect.y = particle->GetModel()->GetPosition().y - 1.0f;
    rect.w = 2.0f;
    rect.h = 2.0f;
    SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 255);
}

void Renderer::RenderShape(Shape* shape)
{
    Get()->RenderShape_Impl(shape);
}

void Renderer::RenderText(std::string str, float size, Vector2f pos, SDL_Color color)
{
    Get()->RenderText_Impl(str, size, pos, color);
}

void Renderer::RenderLine(Vector2f start, Vector2f end, float thickness, SDL_Color color)
{
    Get()->RenderLine_Impl(start, end, thickness, color);
}

void Renderer::RenderVector(Vector2f position, Vector2f direction, float length)
{
    Get()->RenderVector_Impl(position, direction, length);
}

void Renderer::RenderParticle(Particle* particle)
{
    Get()->RenderParticle_Impl(particle);
}

void Renderer::RenderParticleDetailed(Particle* particle)
{
    Get()->RenderParticleDetailed_Impl(particle);
}

