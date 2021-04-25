#pragma once
#include "PCH.h"

class ConstantBuffer
{
public:
	ConstantBuffer()
	{
		WorldMatrix = XMMatrixIdentity();
		ViewMatrix = XMMatrixIdentity();
		ProjectionMatrix = XMMatrixIdentity();
	};

	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;
};

class GraphicsDevice
{
private:
	static GraphicsDevice* instance;
	string pixelShaderPath;
	string vertexShaderPath;
	bool isInitialised;

	IDXGISwapChain*				swapChain;
	D3D_FEATURE_LEVEL			featureLevel;
	D3D_DRIVER_TYPE				driverType;
	ID3D11VertexShader*			vertexShader;
	ID3D11PixelShader*			pixelShader;
	ID3D11InputLayout*			vertexLayout;
	ID3D11Device*				d3dDevice;
	ID3D11DeviceContext*		d3dContext;
	ID3D11RenderTargetView*		renderTarget;
	ID3D11Buffer*				constantBuffer;
	ID3D11Texture2D*			depthStencilTexture;
	ID3D11DepthStencilState*	depthStencilState;
	ID3D11DepthStencilView*		depthStencilView;

	HRESULT InitShaders(WCHAR* pixelShaderFile, WCHAR* vertexShaderFile);
	HRESULT InitInputLayout(ID3DBlob* vertexShaderBlob);
	HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	GraphicsDevice();
	~GraphicsDevice();

	ID3D11Device* GetDevice_Impl();
	ID3D11DeviceContext* GetContext_Impl();
	ID3D11RenderTargetView* const* GetRenderTarget_Impl();
	ID3D11DepthStencilView* GetDepthStencilView_Impl();
	ID3D11Buffer* GetConstantBuffer_Impl();
	IDXGISwapChain* GetSwapChain_Impl();

public:
	int WindowWidth;
	int WindowHeight;


	HRESULT Init(HINSTANCE hInstance, HWND hWindow, int width, int height, string PS_Path, string VS_Path);
	static GraphicsDevice* Get();

	bool									GetIsInitialised();

	static ID3D11Device*					GetDevice();
	static ID3D11DeviceContext*				GetContext();
	//Returns back buffer render target
	static ID3D11RenderTargetView* const *  GetRenderTarget();
	static ID3D11DepthStencilView*			GetDepthStencilView();
	static ID3D11Buffer*					GetConstantBuffer();
	static IDXGISwapChain*					GetSwapChain();

	void									VSSetShader();
	void									PSSetShader();
};

