//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/gfxmanager.h"
#include "../Source/GFXRenderTexture.h"

GFXRenderTexture::GFXRenderTexture()
{
    BaseConstructor();
}

GFXRenderTexture::GFXRenderTexture(const GFXRenderTexture& )
{
    // TO THINK
    // shall i allow for cloning of GFX render texture?
    BaseConstructor();
}

void GFXRenderTexture::BaseConstructor()
{
    m_RenderTargetTexture = 0;
    m_RenderTargetView    = 0;
    m_ShaderResourceView  = 0;
    m_GFXManager          = GFXManager::GetInstance();
}

GFXRenderTexture::~GFXRenderTexture()
{
    SafeRelease(m_RenderTargetTexture);
    SafeRelease(m_RenderTargetView);
    SafeRelease(m_ShaderResourceView);
    m_GFXManager = nullptr;
}

bool GFXRenderTexture::Initialize(int textureWidth, int textureHeight)
{
    ID3D11Device* device = GFXManager::GetInstance()->GetD3DDevice();

    D3D11_TEXTURE2D_DESC textureDesc;
    HRESULT result;
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;


    // Initialize the render target texture description.
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // Setup the render target texture description.
    textureDesc.Width = textureWidth;
    textureDesc.Height = textureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the render target texture.
    result = device->CreateTexture2D(&textureDesc, NULL, &m_RenderTargetTexture);
    if (FAILED(result))
    {
        return false;
    }

    // Setup the description of the render target view.
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    // Create the render target view.
    result = device->CreateRenderTargetView(m_RenderTargetTexture, &renderTargetViewDesc, &m_RenderTargetView);
    if (FAILED(result))
    {
        return false;
    }

    // Setup the description of the shader resource view.
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    // Create the shader resource view.
    result = device->CreateShaderResourceView(m_RenderTargetTexture, &shaderResourceViewDesc, &m_ShaderResourceView);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void GFXRenderTexture::Shutdown()
{
    if (m_ShaderResourceView)
    {
        m_ShaderResourceView->Release();
        m_ShaderResourceView = 0;
    }

    if (m_RenderTargetView)
    {
        m_RenderTargetView->Release();
        m_RenderTargetView = 0;
    }

    if (m_RenderTargetTexture)
    {
        m_RenderTargetTexture->Release();
        m_RenderTargetTexture = 0;
    }

    return;
}

void GFXRenderTexture::SetRenderTarget(ID3D11DepthStencilView* depthStencilView)
{
    // Bind the render target view and depth stencil buffer to the output render pipeline.
    ID3D11DeviceContext* deviceContext = m_GFXManager->GetD3DDeviceContext();
    deviceContext->OMSetRenderTargets(1, &m_RenderTargetView, depthStencilView);

    return;
}

void GFXRenderTexture::ClearRenderTarget(ID3D11DepthStencilView* depthStencilView,
    float red, float green, float blue, float alpha)
{
    float color[4] = { red, green, blue, alpha };
    ID3D11DeviceContext* deviceContext = m_GFXManager->GetD3DDeviceContext();
    // Clear the back buffer.
    deviceContext->ClearRenderTargetView(m_RenderTargetView, color);

    // Clear the depth buffer.
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    return;
}

ID3D11ShaderResourceView* GFXRenderTexture::GetShaderResourceView()
{
    return m_ShaderResourceView;
}

ID3D11RenderTargetView* GFXRenderTexture::GetRenderTargetView()
{
    return m_RenderTargetView;
}