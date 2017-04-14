//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/GFXPostProcessingFilter.h"

GFXPostProcessingFilter::~GFXPostProcessingFilter()
{
    SafeRelease(m_pVertexShader);
    SafeRelease(m_pPixelShader);
    SafeRelease(m_pVertexLayout);
}

bool GFXPostProcessingFilter::SetupShaders(std::wstring path)
{
    SafeRelease(m_pVertexShader);
    SafeRelease(m_pPixelShader);
    SafeRelease(m_pVertexLayout);

    // pre cache
    ID3D11Device* pd3dDevice = GFXManager::GetInstance()->GetD3DDevice();

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    HRESULT hr = GFXManager::CompileShaderFromFile(path.c_str(), "VS", "vs_4_0", &pVSBlob);
    if (FAILED(hr))
    {
        return false;
    }

    // Create the vertex shader
    hr = pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return false;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    hr = pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), &m_pVertexLayout);
    pVSBlob->Release();
    if (FAILED(hr))
    {
        return false;
    }

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    hr = GFXManager::CompileShaderFromFile(path.c_str(), "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        return false;
    }

    // Create the pixel shader
    hr = pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader);
    pPSBlob->Release();
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

ID3D11VertexShader* GFXPostProcessingFilter::GetVertexShader()
{
    return m_pVertexShader;
}

ID3D11PixelShader*  GFXPostProcessingFilter::GetPixelShader()
{
    return m_pPixelShader;
}

ID3D11InputLayout*  GFXPostProcessingFilter::GetInputLayout()
{
    return m_pVertexLayout;
}

bool GFXPostProcessingFilter::Init(int textureWidth, int textureHeight)
{
    if (!Initialize(textureWidth, textureHeight))
    {
        return false;
    }
    return InitializeInternal();
}