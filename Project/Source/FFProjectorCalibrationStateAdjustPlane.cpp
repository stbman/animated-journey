//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include <opencv2/opencv.hpp>

#include "../Source/FFProjectorCalibrationStateAdjustPlane.h"
#include "../Source/AJFactoryManager.h"
#include "../Source/AJObject.h"
#include "../Source/AJProjectorCalibrationState.h"
#include "../Source/AJKeyboardManager.h"
#include "../Source/gfxHelper.h"

FFProjectorCalibrationStateAdjustPlane::FFProjectorCalibrationStateAdjustPlane(std::vector<AJPointPairGenerator::PointPair>& vectorList)
{
    m_FirstPlane = vectorList;
}

FFProjectorCalibrationStateAdjustPlane::~FFProjectorCalibrationStateAdjustPlane()
{
}

void FFProjectorCalibrationStateAdjustPlane::OnEnter()
{
    std::wstring texturepath = L"../Resource/adjustplane.dds";
    AJObject* uielement = AJFactory::CreateUIObject(texturepath, 0.75f);
    m_ObjList.push_back(uielement);

    DirectX::XMVECTOR pos = GFXHelper::ConvertNormalizedToScreenSpace(0.5f, 0.5f, 0.0f);
    uielement->SetGlobalPosition(pos);
}

void FFProjectorCalibrationStateAdjustPlane::OnExit()
{
    for each(AJObject* obj in m_ObjList)
    {
        delete obj;
    }

    m_ObjList.clear();
}

void FFProjectorCalibrationStateAdjustPlane::Update(float dt)
{
    dt;

    if (AJKeyboardManager::GetInstance()->Triggered(AJKeyboardManager::Key::KEY_SPACE))
    {
        // move on 
        AJProjectorCalibrationState* projectorCalibrationState = new AJProjectorCalibrationState(m_FirstPlane);
        AJFlowManager::GetInstance()->ReplaceState(projectorCalibrationState);
    }
}