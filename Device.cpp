#include "pch.h"
#include "Device.h"

GraphicsDevice* GraphicsDevice::instance;

GraphicsDevice::GraphicsDevice()
{
    driverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_NULL;
    featureLevel = (D3D_FEATURE_LEVEL)0;
    swapChain = nullptr;
    renderTarget = nullptr;
    constantBuffer = nullptr;
    vertexShader = nullptr;
    pixelShader = nullptr;
    vertexLayout = nullptr;
    d3dDevice = nullptr;
    d3dContext = nullptr;
    depthStencilTexture = nullptr;
    depthStencilState = nullptr;
    depthStencilView = nullptr;
    isInitialised = false;
    WindowWidth = 0;
    WindowHeight = 0;
}

GraphicsDevice::~GraphicsDevice()
{
    driverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_NULL;
    featureLevel = (D3D_FEATURE_LEVEL)0;
    WindowWidth = 0;
    WindowHeight = 0;

    swapChain->Release();
    swapChain = nullptr;
    renderTarget->Release();
    renderTarget = nullptr;
    constantBuffer->Release();
    constantBuffer = nullptr;
    vertexShader->Release();
    vertexShader = nullptr;
    pixelShader->Release();
    pixelShader = nullptr;
    vertexLayout->Release();
    vertexLayout = nullptr;
    d3dDevice->Release();
    d3dDevice = nullptr;
    d3dContext->Release();
    d3dContext = nullptr;

    isInitialised = false;
}

HRESULT GraphicsDevice::InitShaders(WCHAR* pixelShaderFile, WCHAR* vertexShaderFile)
{
    HRESULT hr;
    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;

    hr = CompileShaderFromFile(pixelShaderFile, "ps_main", "ps_5_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"Pixel Shader error. The FX file cannot be compiled.", L"Error", MB_OK);
        return hr;
    }

    // Create the pixel shader
    hr = d3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pixelShader);
    pPSBlob->Release();
    pPSBlob = nullptr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(vertexShaderFile, "vs_main", "vs_5_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"Vertex Shader error. The FX file cannot be compiled.", L"Error", MB_OK);
        return hr;
    }
    // Create the vertex shader
    hr = d3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return hr;
    }

    if (FAILED(hr))
        return hr;

    hr = InitInputLayout(pVSBlob);
    if (FAILED(hr))
        return hr;

    pVSBlob->Release();
    pVSBlob = nullptr;

    return hr;
}

HRESULT GraphicsDevice::InitInputLayout(ID3DBlob* vertexShaderBlob)
{
    HRESULT hr = S_OK;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        /* World Position      */ { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        /* Texture Coordinates */ { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        /* Instance Position   */ { "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };

   /* D3D11_INPUT_ELEMENT_DESC layout[3];

    layout[0].SemanticName = "POSITION";
    layout[0].SemanticIndex = 0;
    layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    layout[0].InputSlot = 1;
    layout[0].AlignedByteOffset = 0;
    layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout[0].InstanceDataStepRate = 1;

    layout[1].SemanticName = "TEXCOORD";
    layout[1].SemanticIndex = 0;
    layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    layout[1].InputSlot = 1;
    layout[1].AlignedByteOffset = 0;
    layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout[1].InstanceDataStepRate = 1;

    layout[2].SemanticName = "TEXCOORD";
    layout[2].SemanticIndex = 1;
    layout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    layout[2].InputSlot = 1;
    layout[2].AlignedByteOffset = 0;
    layout[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    layout[2].InstanceDataStepRate = 1;*/


    UINT numElements = ARRAYSIZE(layout);
    // Create the input layout
    hr = d3dDevice->CreateInputLayout(layout, numElements, vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(), &vertexLayout);
    vertexShaderBlob->Release();

    if (FAILED(hr))
        return hr;

    // Set the input layout
    d3dContext->IASetInputLayout(vertexLayout);

    return hr;
}

HRESULT GraphicsDevice::CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
    #if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
    #endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT GraphicsDevice::Init(HINSTANCE hInstance, HWND hWindow, int width, int height, string PS_Path, string VS_Path)
{
    //Standard DirectX initialisation
    vertexShaderPath = VS_Path;
    pixelShaderPath = PS_Path;

    HRESULT hr = S_OK;

    WindowWidth = width;
    WindowHeight = height;

    UINT createDeviceFlags = 0;

    #ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = WindowWidth;
    sd.BufferDesc.Height = WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWindow;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            driverType,
            nullptr,
            createDeviceFlags,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            &sd,
            &swapChain,
            &d3dDevice,
            &featureLevel,
            &d3dContext);

        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = d3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &renderTarget);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;


    //TODO : Depth Stencil
    //Depth stencil buffer
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = WindowWidth;
    descDepth.Height = WindowHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = d3dDevice->CreateTexture2D(&descDepth, NULL, &depthStencilTexture);

    //depth stencil state
     D3D11_DEPTH_STENCIL_DESC dsDesc;
     ZeroMemory(&dsDesc, sizeof(dsDesc));
    // Depth test parameters
     dsDesc.DepthEnable = false;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    // Stencil test parameters
    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;
    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create depth stencil state
    d3dDevice->CreateDepthStencilState(&dsDesc, &depthStencilState);
    d3dContext->OMSetDepthStencilState(depthStencilState, 1);

    //Depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    // Create the depth stencil view
    hr = d3dDevice->CreateDepthStencilView(
        depthStencilTexture, 
        &descDSV,
        &depthStencilView);

    // Bind the depth stencil view
    d3dContext->OMSetRenderTargets(1, &renderTarget, depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)(WindowWidth);
    vp.Height = (FLOAT)(WindowHeight);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    d3dContext->RSSetViewports(1, &vp);

    std::wstring psPath_w = std::wstring(pixelShaderPath.begin(), pixelShaderPath.end());
    std::wstring vsPath_w = std::wstring(vertexShaderPath.begin(), vertexShaderPath.end());
    InitShaders(&psPath_w[0], &vsPath_w[0]);

    // Set primitive topology
    d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = d3dDevice->CreateBuffer(&bd, nullptr, &constantBuffer);

    if (FAILED(hr))
        return hr;

    if (hr == S_OK)
        isInitialised = true;

	return hr;
}

GraphicsDevice* GraphicsDevice::Get()
{
    if (instance == nullptr)
        instance = new GraphicsDevice();

    return instance;
}

void GraphicsDevice::VSSetShader()
{
    d3dContext->VSSetShader(vertexShader, nullptr, 0);
}

void GraphicsDevice::PSSetShader()
{
    d3dContext->PSSetShader(pixelShader, nullptr, 0);
}

bool GraphicsDevice::GetIsInitialised()
{
    return isInitialised;
}

ID3D11Buffer* GraphicsDevice::GetConstantBuffer()
{
    return Get()->GetConstantBuffer_Impl();
}

ID3D11DepthStencilView* GraphicsDevice::GetDepthStencilView()
{
    return Get()->GetDepthStencilView_Impl();
}

ID3D11Device* GraphicsDevice::GetDevice()
{
    return Get()->GetDevice_Impl();
}

IDXGISwapChain* GraphicsDevice::GetSwapChain()
{
    return Get()->GetSwapChain_Impl();
}

ID3D11DeviceContext* GraphicsDevice::GetContext()
{
    return Get()->GetContext_Impl();
}

ID3D11RenderTargetView* const* GraphicsDevice::GetRenderTarget()
{
    return Get()->GetRenderTarget_Impl();
}

ID3D11Device* GraphicsDevice::GetDevice_Impl()
{
    return d3dDevice;
}

ID3D11DeviceContext* GraphicsDevice::GetContext_Impl()
{
    return d3dContext;
}

ID3D11RenderTargetView* const* GraphicsDevice::GetRenderTarget_Impl()
{
    return &renderTarget;
}

ID3D11DepthStencilView* GraphicsDevice::GetDepthStencilView_Impl()
{
    return depthStencilView;
}

ID3D11Buffer* GraphicsDevice::GetConstantBuffer_Impl()
{
    return constantBuffer;
}

IDXGISwapChain* GraphicsDevice::GetSwapChain_Impl()
{
    return swapChain;
}