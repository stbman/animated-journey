//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include <opencv2/opencv.hpp>

#include "../Source/AJProjectorSearchState.h"
#include "../Source/AJFactoryManager.h"
#include "../Source/AJObject.h"
#include "../Source/gfxHelper.h"
#include "../Source/AJColorCalibrationState.h"
#include "../Source/AJPointPairComponent.h"
#include "../Source/AJMathHelper.h"
#include "../Source/AJKinectHelper.h"

AJProjectorSearchState::AJProjectorSearchState()
{
}

AJProjectorSearchState::~AJProjectorSearchState()
{
}

void AJProjectorSearchState::OnEnter()
{
    m_TimeLeft = 2.0f;
}

void AJProjectorSearchState::OnExit()
{
    for each(AJObject* obj in m_ObjList)
    {
        delete obj;
    }

    m_ObjList.clear();

    SafeDelete(m_PointPair);
}

void AJProjectorSearchState::Update(float dt)
{
}

cv::Mat AJProjectorSearchState::GetBaseColourFrame()
{
    return m_BaseColorFrame;
}

cv::Mat AJProjectorSearchState::GetBaseDepthFrame()
{
    return m_BaseDepthFrame;
}

cv::Mat AJProjectorSearchState::GetBaseCameraPointFrame()
{
    return m_BaseCameraPointFrame;
}

// Find transformation coefficients 
// M x = b, where x is the transformation coeff
void AJProjectorSearchState::SolveForTransformationCoeff(std::vector<XMFLOAT3>KinectPoints,
                                        std::vector<XMFLOAT3>ProjectorPoints)
{
}

// For projecting subsequent points after calibration coeff solved
XMFLOAT3 AJProjectorSearchState::ConvertKinectToProjector(XMFLOAT3 KinectPoint)
{
    XMFLOAT3 ProjectorPoint;

    return ProjectorPoint;
}

float AJProjectorSearchState::SolveForProjectorFOV(XMFLOAT3 TopRight, XMFLOAT3 TopLeft,
    XMFLOAT3 BottomRight, XMFLOAT3 BottomLeft)
{


    return m_ProjectorFOV;
}

void AJProjectorSearchState::CalculateProjectorParameters()
{
}

void AJProjectorSearchState::CalculateProjectorCenterPos()
{
}

void AJProjectorSearchState::CalculateProjectorUpPos()
{
}

void AJProjectorSearchState::CalculateProjectorLookAt()
{
}

void AJProjectorSearchState::TransformationVecToMat()
{
}