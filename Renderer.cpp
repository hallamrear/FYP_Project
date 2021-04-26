#include "PCH.h"
#include "Renderer.h"
#include "Particle.h"

Renderer* Renderer::instance;

Renderer::Renderer()
{
    WindowHeight = 0;
    WindowWidth = 0;
    constantBuffer = ConstantBuffer();
    isInitialised = false;
    linearSampler = nullptr;
    NoBlendState = nullptr;
    NoCullSolidFillState = nullptr;
    rtTex = nullptr;
    rtView = nullptr;
    rtSRV = nullptr;
    constantBuffer;
    spriteBatch = nullptr;
    ViewPosition = XMFLOAT2();
}

Renderer::~Renderer()
{
    linearSampler->Release();
    linearSampler = nullptr;

    NoBlendState->Release();
    NoBlendState = nullptr;

    rtTex->Release();
    rtTex = nullptr;
    rtView->Release();
    rtView = nullptr;
    rtSRV->Release();
    rtSRV = nullptr;
}


HRESULT Renderer::Init(HINSTANCE hInstance, HWND hWindow, int width, int height)
{
    HRESULT hr;

    WindowWidth = width;
    WindowHeight = height;

    if(GraphicsDevice::Get()->GetIsInitialised() == false)
        GraphicsDevice::Get()->Init(hInstance, hWindow, width, height, PS_SHADER, VS_SHADER);

    //TODO : alter
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.DepthBias = 0.0f;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.DepthClipEnable = true;
    hr = GraphicsDevice::GetDevice()->CreateRasterizerState(&rastDesc, &NoCullSolidFillState);
    if (FAILED(hr))
        return hr;
    GraphicsDevice::GetContext()->RSSetState(NoCullSolidFillState);

    //tex sampler
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = GraphicsDevice::GetDevice()->CreateSamplerState(&sampDesc, &linearSampler);

    if (FAILED(hr))
        return hr;

    //Blend state
    D3D11_BLEND_DESC blendDesc;
    D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDescription;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    ZeroMemory(&renderTargetBlendDescription, sizeof(renderTargetBlendDescription));
    blendDesc.RenderTarget[0].BlendEnable = false;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
   
    renderTargetBlendDescription.BlendEnable = true;
    renderTargetBlendDescription.SrcBlend = D3D11_BLEND_SRC_COLOR;
    renderTargetBlendDescription.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    renderTargetBlendDescription.BlendOp = D3D11_BLEND_OP_ADD;
    renderTargetBlendDescription.SrcBlendAlpha = D3D11_BLEND_ONE;
    renderTargetBlendDescription.DestBlendAlpha = D3D11_BLEND_ZERO;
    renderTargetBlendDescription.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    renderTargetBlendDescription.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.RenderTarget[0] = renderTargetBlendDescription;

    hr = GraphicsDevice::GetDevice()->CreateBlendState(&blendDesc, &NoBlendState);

    if (FAILED(hr))
        return hr;

    //todo: remove maybe
    float blendFactor[4] = { 0.75f, 0.75f, 0.75f, 1.0f };
    UINT sampleMask = 0xffffffff;
    GraphicsDevice::GetContext()->OMSetBlendState(NoBlendState, blendFactor, sampleMask);

    //Create texture for render and shader
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.Width = GraphicsDevice::Get()->WindowWidth;
    textureDesc.Height = GraphicsDevice::Get()->WindowHeight;
    textureDesc.ArraySize = 1;
    hr = GraphicsDevice::GetDevice()->CreateTexture2D(&textureDesc, NULL, &rtTex);

    if (FAILED(hr))
        return hr;

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.Texture2D.MipSlice = 0;
    hr = GraphicsDevice::GetDevice()->CreateRenderTargetView(rtTex, &renderTargetViewDesc, &rtView);

    if (FAILED(hr))
        return hr;

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    hr = GraphicsDevice::GetDevice()->CreateShaderResourceView(rtTex, &shaderResourceViewDesc, &rtSRV);

    if (FAILED(hr))
        return hr;

    constantBuffer.WorldMatrix = XMMatrixTranspose(XMMatrixIdentity());

    XMVECTOR Pos = XMVectorSet(WORLD_SIZE.x / 2, WORLD_SIZE.y / 2, -1.0f, 0.0f);
    XMVECTOR Target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMFLOAT4X4 tempViewMatrix;
    XMStoreFloat4x4(&tempViewMatrix, XMMatrixLookAtLH(Pos, Target, Up));
    constantBuffer.ViewMatrix = XMMatrixTranspose(XMLoadFloat4x4(&tempViewMatrix));

    XMFLOAT4X4 tempProjMatrix;
    XMStoreFloat4x4(&tempProjMatrix, XMMatrixOrthographicLH(GraphicsDevice::Get()->WindowWidth, GraphicsDevice::Get()->WindowHeight, 0.01f, 200.0f));
    constantBuffer.ProjectionMatrix = XMMatrixTranspose(XMLoadFloat4x4(&tempProjMatrix));

    GraphicsDevice::GetContext()->UpdateSubresource(GraphicsDevice::Get()->GetConstantBuffer(), 0, nullptr, &constantBuffer, 0, 0);

    spriteBatch = new SpriteBatch(GraphicsDevice::GetContext());

    hr = CreateDDSTextureFromFile(
        GraphicsDevice::GetDevice(),
        L"Resources/Edge.dds",
        &outlineResource,
        &outlineTexture);

    isInitialised = true;

    return hr;
}

Renderer* Renderer::Get()
{
    if (instance == nullptr)
        instance = new Renderer();

    return instance;
}

void Renderer::AlterViewPosition(XMFLOAT2 adjustment)
{
    char msg[100];
    sprintf_s(msg, "X: %f, Y: %f\n", ViewPosition.x, ViewPosition.y);
    OutputDebugStringA(msg);

    ViewPosition.x += adjustment.x;
    ViewPosition.y += adjustment.y;
    IsViewMatrixDirty = true;
}

void Renderer::PrepareFrame()
{
    float ClearColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GraphicsDevice::GetContext()->ClearRenderTargetView(*GraphicsDevice::Get()->GetRenderTarget(), ClearColour);
    GraphicsDevice::GetContext()->ClearDepthStencilView(GraphicsDevice::Get()->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    GraphicsDevice::GetContext()->OMSetRenderTargets(1, GraphicsDevice::Get()->GetRenderTarget(), GraphicsDevice::Get()->GetDepthStencilView());


    if (IsViewMatrixDirty)
    {
        XMVECTOR Pos = XMVectorSet(ViewPosition.x, ViewPosition.y, -1.0f, 0.0f);
        XMVECTOR Target = XMVectorSet(ViewPosition.x, ViewPosition.y, 0.0f, 0.0f);
        XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        XMFLOAT4X4 tempViewMatrix;
        XMStoreFloat4x4(&tempViewMatrix, XMMatrixLookAtLH(Pos, Target, Up));
        constantBuffer.ViewMatrix = XMMatrixTranspose(XMLoadFloat4x4(&tempViewMatrix));

        IsViewMatrixDirty = false;
    }
    else
        constantBuffer.ViewMatrix = XMMatrixTranspose(constantBuffer.ViewMatrix);

    constantBuffer.WorldMatrix = XMMatrixTranspose(XMMatrixIdentity());
    constantBuffer.ProjectionMatrix = XMMatrixTranspose(constantBuffer.ProjectionMatrix);

    GraphicsDevice::GetContext()->PSSetSamplers(0, 1, &linearSampler);
    GraphicsDevice::GetContext()->UpdateSubresource(GraphicsDevice::Get()->GetConstantBuffer(), 0, nullptr, &constantBuffer, 0, 0);
    
    spriteBatch->Begin();


    RECT rect = RECT();
    rect.left = 0;
    rect.top = 0;
    rect.right = WindowWidth;
    rect.bottom = WindowHeight;

    spriteBatch->Draw(outlineTexture, rect);
}

void Renderer::PresentFrame()
{
    spriteBatch->End();
    GraphicsDevice::Get()->GetSwapChain()->Present(0, 0);
}

void Renderer::PrepareGeometryRender()
{
    GraphicsDevice::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    GraphicsDevice::Get()->VSSetShader();
    GraphicsDevice::Get()->PSSetShader();
    GraphicsDevice::GetContext()->PSSetSamplers(0, 1, &linearSampler);

    //TODO : find a better way of doing this
    ID3D11Buffer* buffer = GraphicsDevice::Get()->GetConstantBuffer();
    constantBuffer.WorldMatrix = XMMatrixIdentity();
    constantBuffer.WorldMatrix = XMMatrixTranspose(constantBuffer.WorldMatrix);
    GraphicsDevice::GetContext()->UpdateSubresource(buffer, 0, nullptr, &constantBuffer, 0, 0);

    GraphicsDevice::GetContext()->PSSetConstantBuffers(0, 1, &buffer);
    GraphicsDevice::GetContext()->VSSetConstantBuffers(0, 1, &buffer);
}

void Renderer::Render_Impl(Particle* particle)
{
    spriteBatch->Draw(particle->GetTexture(), particle->GetModel()->position);
}

void Renderer::Render(Particle* particle)
{
    Get()->Render_Impl(particle);
}
