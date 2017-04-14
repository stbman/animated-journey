//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/gfxBaseComponent.h"

GFXBaseComponent::~GFXBaseComponent()
{
    // vertex
    SafeRelease(m_VerticesBuffer);
    SafeDelete(m_VerticesData);

    // indices
    SafeRelease(m_IndicesBuffer);
    SafeDelete(m_IndicesData);

    // texture
    SafeRelease(m_pTextureRV);
    SafeRelease(m_pSamplerLinear);

    //constant buffer
    SafeRelease(m_pCBWorldMatrix);

    // shaders
    SafeRelease(m_pVertexShader);
    SafeRelease(m_pPixelShader);
    SafeRelease(m_pVertexLayout);
}

ID3D11ShaderResourceView* GFXBaseComponent::GetTexture()
{
    return m_pTextureRV;
}

void GFXBaseComponent::SetupVertices(const SimpleVertex* data, UINT size)
{
    // If no data or no vertices (size 0), return
    if (   data == nullptr
        || size <= 0)
    {
        return;
    }

    // Create local simple vertices
    m_VerticesData = new SimpleVertex[size];
    if (m_VerticesData == nullptr)
    {
        return;
    }

    m_VerticesCount = size;
    // Copy data into local data variable
    memcpy(m_VerticesData, data, sizeof(SimpleVertex)*size);

    // Set up info of buffer
    D3D11_BUFFER_DESC bd;
    // Default state
    ZeroMemory(&bd, sizeof(bd));
    // Can change buffer, not static
    bd.Usage          = D3D11_USAGE_DYNAMIC;
    // size of graphic memory buffer in bytes
    bd.ByteWidth      = sizeof(SimpleVertex) * m_VerticesCount;
    // Purpose of buffer
    bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    // CPU write only cannot read (so no need set up pipes for read, expensive)
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    // Initialise buffer with local vertices data
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = m_VerticesData;
    // Create hardware gfx buffer with buffer desc, initialising info, save ref of buffer
    HRESULT hr       = GFXManager::GetInstance()->GetD3DDevice()->CreateBuffer(&bd, &InitData, &m_VerticesBuffer);
    if (FAILED(hr))
    {
        SafeDelete(m_VerticesData);
        return;
    }
}

void GFXBaseComponent::SetupIndices(const WORD* data, UINT size)
{
    if (   data == nullptr
        || size <= 0)
    {
        return;
    }

    m_IndicesData = new WORD[size];
    if (m_IndicesData == nullptr)
    {
        return;
    }

    m_IndicesCount = size;
    memcpy(m_IndicesData, data, sizeof(WORD)*size);

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage          = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth      = sizeof(WORD) * m_IndicesCount;
    bd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = m_IndicesData;
    HRESULT hr       = GFXManager::GetInstance()->GetD3DDevice()->CreateBuffer(&bd, &InitData, &m_IndicesBuffer);
    if (FAILED(hr))
    {
        SafeDelete(m_IndicesData);
        return;
    }
}

void GFXBaseComponent::CreateTextureResource(std::wstring path)
{
    SafeRelease(m_pTextureRV);
    CreateDDSTextureFromFile(m_pd3dDevice, path.c_str(), nullptr, &m_pTextureRV);
}

void GFXBaseComponent::SwapTextureResource(ID3D11ShaderResourceView* texture)
{
    m_pTextureRV = texture;
}

void GFXBaseComponent::SwapTextureData()
{
    ID3D11Resource* res = nullptr;
    ID3D11Texture2D* tex = nullptr;
    m_pTextureRV->GetResource(&res);

    m_pImmediateContext->Map(res, 0, D3D11_MAP_WRITE_DISCARD, 0, nullptr);

    m_pImmediateContext->Unmap(res, 0);
}

bool GFXBaseComponent::SetupShaders(std::wstring path)
{
    // Check if not null delete shaders
    SafeRelease(m_pVertexShader);
    SafeRelease(m_pPixelShader);
    SafeRelease(m_pVertexLayout);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    HRESULT hr = GFXManager::CompileShaderFromFile(path.c_str(), "VS", "vs_4_0", &pVSBlob);
    if (FAILED(hr))
    {
        return false;
    }

    // Create the vertex shader
    hr = m_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return false;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    hr = m_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), &m_pVertexLayout);
    pVSBlob->Release();  // Don't need compiled shader already
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
    hr = m_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader);
    pPSBlob->Release();
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool GFXBaseComponent::SetupShaders(ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11InputLayout* inputLayout)
{
    m_pVertexShader = vertexShader;
    m_pPixelShader  = pixelShader;
    m_pVertexLayout = inputLayout;

    return true;
}

void GFXBaseComponent::UpdateVertices(const SimpleVertex* data, UINT size)
{
    if (m_VerticesCount != size)
    {
        SafeRelease(m_VerticesBuffer);
        SafeDelete(m_VerticesData);
        SetupVertices(data, size);
        return;
    }

    memcpy(m_VerticesData, data, sizeof(SimpleVertex) * m_VerticesCount);

    D3D11_MAPPED_SUBRESOURCE ms;
    // Control graphics card 
    m_pImmediateContext->Map(m_VerticesBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, data, sizeof(SimpleVertex)*size);
    m_pImmediateContext->Unmap(m_VerticesBuffer, NULL);
}

void GFXBaseComponent::UpdateIndices(const WORD* data, UINT size)
{
    if (m_IndicesCount != size)
    {
        SafeRelease(m_IndicesBuffer);
        SafeDelete(m_IndicesData);
        SetupIndices(data, size);
        return;
    }

    memcpy(m_IndicesData, data, sizeof(WORD) * m_IndicesCount);

    D3D11_MAPPED_SUBRESOURCE ms;
    m_pImmediateContext->Map(m_IndicesBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, data, sizeof(WORD)*size);
    m_pImmediateContext->Unmap(m_IndicesBuffer, NULL);
}

bool GFXBaseComponent::Init()
{
    m_pd3dDevice        = GFXManager::GetInstance()->GetD3DDevice();
    m_pImmediateContext = GFXManager::GetInstance()->GetD3DDeviceContext();

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CBWorldMatrix);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HRESULT hr = m_pd3dDevice->CreateBuffer(&bd, nullptr, &m_pCBWorldMatrix);
    if (FAILED(hr))
    {
        return false;
    }

    CBWorldMatrix cbWorld;
    cbWorld.mWorld = DirectX::XMMatrixIdentity();
    m_pImmediateContext->UpdateSubresource(m_pCBWorldMatrix, 0, nullptr, &cbWorld, 0, 0);

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = m_pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void GFXBaseComponent::GFXUpdate()
{
    // compute resource
    CBWorldMatrix cbWorld;
    cbWorld.mWorld = XMMatrixTranspose(GetOwner()->GetGlobalMatrix());
    m_pImmediateContext->UpdateSubresource(m_pCBWorldMatrix, 0, nullptr, &cbWorld, 0, 0);
}

void GFXBaseComponent::Render()
{
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers(0, 1, &m_VerticesBuffer, &stride, &offset);
    // index buffer stride 16 bits (word)
    m_pImmediateContext->IASetIndexBuffer(m_IndicesBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_pImmediateContext->IASetInputLayout(m_pVertexLayout);
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    
    m_pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
    // Set shader's world matrix at reg b2
    m_pImmediateContext->VSSetConstantBuffers(2, 1, &m_pCBWorldMatrix);
    m_pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);
    m_pImmediateContext->PSSetConstantBuffers(2, 1, &m_pCBWorldMatrix);
    // Set texture at reg t0
    m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTextureRV);
    // Set sampler at s0
    m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerLinear);

    m_pImmediateContext->DrawIndexed(m_IndicesCount, 0, 0);
    return;
}
