//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_GFX_BASECOMPONENT_H
#define AJ_GFX_BASECOMPONENT_H
//---------------------------------------------------------------------------

#include "../Source/AJBaseComponent.h"
#include "../Source/gfxheader.h"

class GFXBaseComponent : public AJBaseComponent
{
private:
    // vertex
    UINT                      m_VerticesCount     = 0;
    SimpleVertex*             m_VerticesData      = nullptr;
    ID3D11Buffer*             m_VerticesBuffer    = nullptr;

    // index
    UINT                      m_IndicesCount      = 0;
    WORD*                     m_IndicesData       = nullptr;
    ID3D11Buffer*             m_IndicesBuffer     = nullptr;

    // texture
    ID3D11ShaderResourceView* m_pTextureRV        = nullptr;
    ID3D11SamplerState*       m_pSamplerLinear    = nullptr;

    // world matrix
    ID3D11Buffer*             m_pCBWorldMatrix    = nullptr;

    // shaders
    ID3D11VertexShader*       m_pVertexShader     = nullptr;
    ID3D11PixelShader*        m_pPixelShader      = nullptr;
    ID3D11InputLayout*        m_pVertexLayout     = nullptr;

    // Cache
    ID3D11Device*             m_pd3dDevice        = nullptr;
    ID3D11DeviceContext*      m_pImmediateContext = nullptr;
public:
    ~GFXBaseComponent();

    ID3D11ShaderResourceView* GetTexture();

    void SetupVertices(const SimpleVertex* data, UINT size);
    void SetupIndices(const WORD* data, UINT size);
    void CreateTextureResource(std::wstring path);
    void SwapTextureResource(ID3D11ShaderResourceView* texture);
    void SwapTextureData();
    bool SetupShaders(std::wstring path);
    bool SetupShaders(ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11InputLayout* inputLayout);
    void UpdateVertices(const SimpleVertex* data, UINT size);
    void UpdateIndices(const WORD* data, UINT size);
    bool Init();
    void virtual GFXUpdate();
    void Render();

};

#endif