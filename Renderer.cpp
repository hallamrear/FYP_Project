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

SDL_GPUShader* m_ComputeShader;
SDL_GPUComputePass* m_CurrentComputePass;
SDL_GPUCommandBuffer* m_CommandBuffer;
SDL_GPUComputePipeline* m_ComputePipeline;
char* m_ShaderByteCode;
size_t m_ShaderByteCodeSize;
SDL_GPUBuffer* m_GPUBuffer;
SDL_GPUTransferBuffer* m_TransferBuffer;

void Renderer::ComputePass()
{
    m_CommandBuffer = SDL_AcquireGPUCommandBuffer(m_GPUDevice);

    float* transferData = (float*)SDL_MapGPUTransferBuffer(m_GPUDevice, m_TransferBuffer, true);
    memcpy(&test[0], transferData, sizeof(float) * test.size());
    SDL_UnmapGPUTransferBuffer(m_GPUDevice, m_TransferBuffer);

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(m_CommandBuffer);

    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = m_TransferBuffer;
    location.offset = 0;

    SDL_GPUBufferRegion region{};
    region.buffer = m_GPUBuffer;
    region.size = sizeof(float) * test.size();
    region.offset = 0;

    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

    SDL_EndGPUCopyPass(copyPass);

    SDL_GPUStorageBufferReadWriteBinding storageBinding{};
    storageBinding.buffer = m_GPUBuffer;
    storageBinding.cycle = true;
    m_CurrentComputePass = SDL_BeginGPUComputePass(m_CommandBuffer, nullptr, 0, &storageBinding, 1);
    SDL_BindGPUComputePipeline(m_CurrentComputePass, m_ComputePipeline);

    //PUSH DATA HERE

    SDL_DispatchGPUCompute(m_CurrentComputePass, 8, 1, 1);

    SDL_EndGPUComputePass(m_CurrentComputePass);
    m_CurrentComputePass = nullptr;

    copyPass = SDL_BeginGPUCopyPass(m_CommandBuffer);
    SDL_DownloadFromGPUBuffer(copyPass, &region, &location);

    SDL_EndGPUCopyPass(copyPass);
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

    m_Renderer = SDL_CreateRenderer(GraphicsDevice::GetWindow(), nullptr);

    //m_ParticleImage = IMG_Load("Particle.png");

    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_DXIL;

    bool debug = false;
#ifdef  _DEBUG
    debug = true;
#endif //  _DEBUG

    m_GPUDevice = SDL_CreateGPUDevice(format, debug, "direct3d12");
    m_CommandBuffer = SDL_AcquireGPUCommandBuffer(m_GPUDevice);

    SDL_GPUComputePipelineCreateInfo createInfo{};
    memset(&createInfo, 0, sizeof(SDL_GPUComputePipelineCreateInfo));

    //Load file.
    std::ifstream stream("ComputeShader.cso", std::ios::binary);

    char* bytes = nullptr;
    size_t size = 0;

    //if (stream.good() && stream.is_open())
    //{
    //    stream.seekg(std::ios::end);
    //    size = stream.tellg();
    //    bytes = new char[size];
    //    //Seek end
    //    stream.seekg(0, std::ios::end);
    //    //Get size from current position
    //    int size = (int)stream.tellg();
    //    //Go back to the start for read.
    //    stream.seekg(0, std::ios::beg);
    //    stream.read(bytes, size);
    //    stream.close();
    //}

    //if (bytes != nullptr)
    //{
    //    m_ShaderByteCode = bytes;
    //    m_ShaderByteCodeSize = size;
    //}
    //else
    //{
    //    throw;
    //}

    m_ShaderByteCode = (char*)SDL_LoadFile("ComputeShader.cso", &m_ShaderByteCodeSize);

    createInfo.code = (Uint8*)m_ShaderByteCode;
    createInfo.code_size = m_ShaderByteCodeSize;
    createInfo.entrypoint = "main";
    createInfo.format = format;
    createInfo.threadcount_x = 8;
    createInfo.threadcount_y = 1;
    createInfo.threadcount_z = 1;
    createInfo.num_readwrite_storage_buffers = 1;
    createInfo.num_readwrite_storage_textures = 0;
    createInfo.num_readonly_storage_textures = 0;
    createInfo.num_readonly_storage_buffers = 0;

    m_ComputePipeline = SDL_CreateGPUComputePipeline(m_GPUDevice, &createInfo);

    std::string er = SDL_GetError();

    /*SDL_GPUShaderCreateInfo shaderCreateInfo{};
    memset(&shaderCreateInfo, 0, sizeof(SDL_GPUShaderCreateInfo));

    shaderCreateInfo.code = (Uint8*)m_ShaderByteCode;
    shaderCreateInfo.code_size = m_ShaderByteCodeSize;
    shaderCreateInfo.entrypoint = "main";
    shaderCreateInfo.format = format;
    shaderCreateInfo.num_samplers = 0;
    shaderCreateInfo.num_storage_buffers = 1;
    shaderCreateInfo.num_storage_textures = 0;
    shaderCreateInfo.num_uniform_buffers = 0;
    shaderCreateInfo.props = 0;
    shaderCreateInfo.stage = SDL_GPU_SHADERSTAGE;

    m_ComputeShader = SDL_CreateGPUShader(m_GPUDevice, &shaderCreateInfo);*/

    SDL_GPUBufferCreateInfo bufferInfo{ 0 };
    bufferInfo.size = sizeof(float) * test.size();
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_WRITE | SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_READ;
    m_GPUBuffer = SDL_CreateGPUBuffer(m_GPUDevice, &bufferInfo);

    SDL_GPUTransferBufferCreateInfo transferBufferInfo = {};
    transferBufferInfo.size = bufferInfo.size;
    transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    m_TransferBuffer = SDL_CreateGPUTransferBuffer(m_GPUDevice, &transferBufferInfo);




    //SDL_ReleaseGPUGraphicsPipeline(context->Device, DrawPipeline);
    //SDL_ReleaseGPUTexture(context->Device, Texture);
    //SDL_ReleaseGPUSampler(context->Device, Sampler);
    //SDL_ReleaseGPUBuffer(context->Device, VertexBuffer);


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

