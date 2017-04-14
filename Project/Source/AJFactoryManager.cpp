//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJFactoryManager.h"
#include "../Source/gfxHelper.h"

#include "../Source/AJObject.h"
#include "../Source/gfxcomponent.h"
#include "../Source/GFXUIComponent.h"
#include "../Source/AJFaceComponent.h"
#include "../Source/AJPointPairComponent.h"

namespace AJFactory
{
    AJObject* CreateFaceObject(int index)
    {
        AJObject* newObj = new AJObject();
        GFXComponent* gfxComp = new GFXComponent();
        newObj->AddComponent(gfxComp);
        gfxComp->Init();
        gfxComp->SetupShaders(L"../Shader/GFXShaderTexture.fx");
        GFXHelper::ResizeQuad(gfxComp, 0.0f, 0.0f, 0.0f, 0.0f);
        gfxComp->CreateTextureResource(L"../Resource/ironman.dds");

        AJFaceComponent* faceComp = new AJFaceComponent();
        faceComp->Init(index);
        newObj->AddComponent(faceComp);

        return newObj;
    }

    AJObject* CreatePointPairObject(std::vector<AJPointPairGenerator::PointPair>& vectorList, cv::Mat& baseColorFrame)
    {
        AJObject* pointpair = AJFactory::CreateUIObject(L"../Resource/reddot.dds", 0.03f);
        AJPointPairComponent* pointPairComponent = new AJPointPairComponent(vectorList, baseColorFrame);
        pointpair->AddComponent(pointPairComponent);

        pointpair->FindComponent<GFXBaseComponent>()->SetActive(false);

        return pointpair;
    }

    GFXUIComponent* CreateQuadComponent(float left, float right, float bottom, float top)
    {
        GFXUIComponent* gfxUIComp = new GFXUIComponent();
        gfxUIComp->Init();
        
        GFXHelper::ResizeQuad(gfxUIComp, left, right, bottom, top);

        //gfxComp->SetupTexture(L"../Resource/seafloor.dds");
        //TODO: WRITE TEXTURE BINDING :D

        return gfxUIComp;
    }

    AJObject* CreateUIObject(std::wstring texturepath)
    {
        AJObject*       newObj = new AJObject();
        GFXUIComponent* gfxUIComp = CreateQuadComponent(0.0f, 0.0f, 0.0f, 0.0f);
        gfxUIComp->SetupShaders(L"../Shader/GFXShaderTexture.fx");
        gfxUIComp->CreateTextureResource(texturepath);

        UINT width = 0;
        UINT height = 0;

        GFXHelper::GetTextureSize(gfxUIComp->GetTexture(), width, height);

        float halfWidth = static_cast<float>(width)* 0.5f;
        float halfHeight = static_cast<float>(height)* 0.5f;

        GFXHelper::ResizeQuad(gfxUIComp, -halfWidth, halfWidth, -halfHeight, halfHeight);
        newObj->AddComponent(gfxUIComp);

        return newObj;
    }

    AJObject* CreateUIObject(std::wstring texturepath, float normalizedScale)
    {
        AJObject*       newObj = new AJObject();
        GFXUIComponent* gfxUIComp = CreateQuadComponent(0.0f, 0.0f, 0.0f, 0.0f);
        gfxUIComp->SetupShaders(L"../Shader/GFXShaderTexture.fx");
        gfxUIComp->CreateTextureResource(texturepath);

        UINT width  = 0;
        UINT height = 0;

        GFXHelper::GetTextureSize(gfxUIComp->GetTexture(), width, height);

        float screenWidth = (float)GFXManager::GetInstance()->GetWindowWidth();
        float normalizedWidth = screenWidth * normalizedScale;
        float imgAspectRatio = (float)height / (float)width;

        float halfWidth  = normalizedWidth * 0.5f;
        float halfHeight = normalizedWidth * imgAspectRatio * 0.5f;

        GFXHelper::ResizeQuad(gfxUIComp, -halfWidth, halfWidth, -halfHeight, halfHeight);
        newObj->AddComponent(gfxUIComp);

        return newObj;
    }

    AJObject* CreateUIObject(std::wstring texturepath, float left, float right, float bottom, float top)
    {
        AJObject*       newObj    = new AJObject();
        GFXUIComponent* gfxUIComp = CreateQuadComponent(left, right, bottom, top);
        gfxUIComp->SetupShaders(L"../Shader/GFXShaderTexture.fx");
        gfxUIComp->CreateTextureResource(texturepath);
        newObj->AddComponent(gfxUIComp);

        return newObj;
    }
}
