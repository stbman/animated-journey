//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef AJ_GFX_RENDER_TEXTURE_H
#define AJ_GFX_RENDER_TEXTURE_H
//---------------------------------------------------------------------------

#include "../Source/predefine.h"

class GFXRenderTexture
{
private:
    ID3D11Texture2D*          m_RenderTargetTexture = nullptr;
    ID3D11RenderTargetView*   m_RenderTargetView    = nullptr;
    ID3D11ShaderResourceView* m_ShaderResourceView  = nullptr;
    GFXManager*               m_GFXManager          = nullptr;

    void BaseConstructor();
public:
    GFXRenderTexture();
    GFXRenderTexture(const GFXRenderTexture&);
    ~GFXRenderTexture();

    bool Initialize(int textureWidth, int textureHeight);
    void Shutdown();

    void SetRenderTarget(ID3D11DepthStencilView*);
    void ClearRenderTarget(ID3D11DepthStencilView*, float, float, float, float);
    ID3D11ShaderResourceView* GetShaderResourceView();
    ID3D11RenderTargetView* GetRenderTargetView();
};

#endif