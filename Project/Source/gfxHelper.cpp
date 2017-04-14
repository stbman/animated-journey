//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/gfxHelper.h"
#include "../Source/gfxBaseComponent.h"
#include "../Source/AJMathHelper.h"

namespace GFXHelper
{
    DirectX::XMVECTOR ConvertScreenSpaceToNormalized(float right, float up, float depth)
    {
        GFXManager* gfxManager = GFXManager::GetInstance();
        return DirectX::XMVectorSet(AJMathHelper::Cursorize(right, 0.0f, (float)gfxManager->GetWindowWidth()),
                                    AJMathHelper::Cursorize(up,    0.0f, (float)gfxManager->GetWindowHeight()),
                                    depth,
                                    0.0f);
    }

    DirectX::XMVECTOR ConvertNormalizedToScreenSpace(float right, float up, float depth)
    {
        GFXManager* gfxManager = GFXManager::GetInstance();
        return DirectX::XMVectorSet(AJMathHelper::Lerp(right, 0.0f, (float)gfxManager->GetWindowWidth()),
                                    AJMathHelper::Lerp(up,    0.0f, (float)gfxManager->GetWindowHeight()),
                                    depth,
                                    0.0f);
    }

    void ResizeQuad(GFXBaseComponent* gfxUIComp, float left, float right, float bottom, float top)
    {
        SimpleVertex vertices[] =
        {
            { XMFLOAT3(left, top, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
            { XMFLOAT3(right, bottom, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
            { XMFLOAT3(left, bottom, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
            { XMFLOAT3(right, top, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) }
        };

        gfxUIComp->UpdateVertices(vertices, sizeof(vertices) / sizeof(SimpleVertex));

        WORD indices[] =
        {
            0, 1, 2,
            0, 3, 1
        };

        gfxUIComp->UpdateIndices(indices, sizeof(indices) / sizeof(WORD));
    }

    bool GetTextureSize(ID3D11ShaderResourceView* texture, UINT& width, UINT& height)
    {
        if (!texture)
        {
            return false;
        }

        ID3D11Resource* resource = nullptr;
        texture->GetResource(&resource);
        if (!resource)
        {
            return false;
        }

        ID3D11Texture2D* textureResource = static_cast<ID3D11Texture2D*>(resource);
        if (!textureResource)
        {
            return false;
        }

        D3D11_TEXTURE2D_DESC desc;
        textureResource->GetDesc(&desc);

        width  = desc.Width;
        height = desc.Height;
        return false;
    }

    void SetVisibility(AJObject* obj, bool visible)
    {
        GFXBaseComponent* gfxComp = obj->FindComponent<GFXBaseComponent>();
        gfxComp->SetActive(visible);
    }
}
