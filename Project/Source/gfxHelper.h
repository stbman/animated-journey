//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_GFX_HELPER_H
#define AJ_GFX_HELPER_H
//---------------------------------------------------------------------------

#include "../Source/gfxheader.h"

struct ID3D11ShaderResourceView;
class GFXBaseComponent;

namespace GFXHelper
{
    DirectX::XMVECTOR ConvertScreenSpaceToNormalized(float left, float up, float depth);
    DirectX::XMVECTOR ConvertNormalizedToScreenSpace(float left, float up, float depth);
    void ResizeQuad(GFXBaseComponent* gfxUIComp, float left, float right, float bottom, float top);
    bool GetTextureSize(ID3D11ShaderResourceView* texture, UINT& width, UINT& height);
    void SetVisibility(AJObject* obj, bool visible);
}

#endif
