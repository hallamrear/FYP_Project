#include "pch.h"
#include "NewRenderer.h"
#include "Particle.h"

ParticleRenderer* ParticleRenderer::instance;

ParticleRenderer::ParticleRenderer()
{
    spriteBatch = new SpriteBatch(GraphicsDevice::GetContext());
}

ParticleRenderer::~ParticleRenderer()
{
    if (spriteBatch != nullptr)
    {
        delete spriteBatch;
        spriteBatch = nullptr;
    }
}

ParticleRenderer* ParticleRenderer::Get()
{
    if (instance == nullptr)
        instance = new ParticleRenderer();

    return instance;
}

HRESULT ParticleRenderer::Init(HWND window, HINSTANCE instance, int width, int height)
{
    HRESULT hr = E_NOTIMPL;

    Get();
    




    return hr;
}

void ParticleRenderer::Shutdown()
{
}

void ParticleRenderer::PrepareFrame()
{
    auto renderTarget = GraphicsDevice::GetRenderTarget();
    auto depthStencil = GraphicsDevice::GetDepthStencilView();

    ID3D11DeviceContext* context = GraphicsDevice::GetContext();
    context->ClearRenderTargetView(*renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, renderTarget, depthStencil);

}

void ParticleRenderer::PresentFrame()
{
    //spriteBatch->End();
    GraphicsDevice::GetSwapChain()->Present(0, DXGI_PRESENT_ALLOW_TEARING);
}

void ParticleRenderer::Render_Impl(Particle* particle)
{
    spriteBatch->Begin();
    spriteBatch->Draw(particle->GetTexture(), particle->model.position);
    spriteBatch->End();
}

void ParticleRenderer::Render(Particle* particle)
{
    Get()->Render_Impl(particle);
}
