//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJPointPairLogicCapture.h"
#include "../Source/gfxBaseComponent.h"
#include "../Source/AJProjectorSearchState.h"
#include "../Source/AJKinectHelper.h"

#define SAVE_IMAGE true

UINT AJPointPairLogicCapture::m_CurrentImage = 0;

AJPointPairLogicCapture::AJPointPairLogicCapture(AJPointPairComponent* parent) : AJPointPairLogicBaseState(parent)
{
    AJBaseState* currentState = AJFlowManager::GetInstance()->GetCurrentState();
    m_BaseColourMat           = GetOwner()->m_BaseColorFrame;
}

AJPointPairLogicCapture::~AJPointPairLogicCapture()
{
}

void AJPointPairLogicCapture::SaveImage(std::string path, cv::Mat image)
{
    if (!SAVE_IMAGE)
    {
        return;
    }

    std::string finalPath = path + std::to_string(m_CurrentImage) + ".jpg";
    imwrite(finalPath.c_str(), image);
}

void AJPointPairLogicCapture::OnEnter()
{
    GetOwner()->GetOwner()->FindComponent<GFXBaseComponent>()->SetActive(true);
    ++m_CurrentImage;
}

void AJPointPairLogicCapture::OnExit()
{
    GetOwner()->GetOwner()->FindComponent<GFXBaseComponent>()->SetActive(false);
}

void AJPointPairLogicCapture::Update(float dt)
{
    m_ScanTimer += dt;
    if (    m_FoundPoint
        || (   m_FrameCounter++ < 5
            || m_ScanTimer      > 0.5f))
    {
        return;
    }

    XMFLOAT3 position;
    cv::Mat colourMat = AJKinect::GetInstance()->m_colorMat;
    cv::Mat spaceMat  = AJKinect::GetInstance()->m_camPosMat;
    m_FoundPoint = AJKinectHelper::ProcessImage(m_BaseColourMat, colourMat, spaceMat, position);

    if (m_FoundPoint)
    {
        m_Result = XMLoadFloat3(&position);
    }

    SaveImage("./SavedImage/frameImage", colourMat);
}

AJPointPairLogicBaseState::Status AJPointPairLogicCapture::GetStatus()
{
    if (m_FoundPoint)
    {
        return AJPointPairLogicBaseState::Status::Done;
    }

    if (m_ScanTimer >= 5.0f)
    {
        return AJPointPairLogicBaseState::Status::Error;
    }

    return AJPointPairLogicBaseState::Status::InProgress;
}

DirectX::XMVECTOR AJPointPairLogicCapture::GetResult()
{
    return m_Result;
}