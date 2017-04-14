//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef AJ_GFX_POSTPROCESSING_FILTER_H
#define AJ_GFX_POSTPROCESSING_FILTER_H
//---------------------------------------------------------------------------

#include "../Source/predefine.h"
#include "../Source/GFXRenderTexture.h"

class GFXPostProcessingFilter : public GFXRenderTexture
{
private:
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader*  m_pPixelShader  = nullptr;
    ID3D11InputLayout*  m_pVertexLayout = nullptr;
protected:
    virtual bool InitializeInternal() { return true; }
public:
    ~GFXPostProcessingFilter();
    bool Init(int textureWidth, int textureHeight);
    virtual void Update() {}

    bool SetupShaders(std::wstring path);
    ID3D11VertexShader* GetVertexShader();
    ID3D11PixelShader*  GetPixelShader();
    ID3D11InputLayout*  GetInputLayout();
};

#endif