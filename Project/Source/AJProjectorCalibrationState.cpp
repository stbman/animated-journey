//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include <opencv2/opencv.hpp>

#include "../Source/AJProjectorCalibrationState.h"
#include "../Source/AJFactoryManager.h"
#include "../Source/AJObject.h"
#include "../Source/gfxHelper.h"
#include "../Source/AJColorCalibrationState.h"
#include "../Source/FFProjectorCalibrationStateAdjustPlane.h"
#include "../Source/AJMathHelper.h"
#include "../Source/AJKinectHelper.h"
#include "../Source/AJKeyboardManager.h"
#include "../Source/AJFaceProjectionState.h"

AJProjectorCalibrationState::AJProjectorCalibrationState()
{
}

AJProjectorCalibrationState::AJProjectorCalibrationState(std::vector<AJPointPairGenerator::PointPair>& vectorList)
{
    m_FirstPlane = vectorList;
}

AJProjectorCalibrationState::~AJProjectorCalibrationState()
{
}

void AJProjectorCalibrationState::OnEnter()
{
}

void AJProjectorCalibrationState::OnExit()
{
    for each(AJObject* obj in m_ObjList)
    {
        delete obj;
    }

    m_ObjList.clear();

    SafeDelete(m_PointPair);
}

void AJProjectorCalibrationState::Update(float dt)
{
    if (AJKeyboardManager::GetInstance()->Triggered(AJKeyboardManager::Key::KEY_BACKSPACE))
    {
        AJFaceProjectionState* faceProjectionState = new AJFaceProjectionState();
        AJFlowManager::GetInstance()->ReplaceState(faceProjectionState);
    }

    dt;

    AJKinect* kinect = AJKinect::GetInstance();
    if (!kinect->m_KinectIsInit)
    {
        return;
    }

    ++m_FrameCounter;
    if (m_FrameCounter < 30)
    {
        return;
    }

    if (!m_CapturedBaseFrame)
    {
        kinect->m_colorMat.copyTo(m_BaseColorFrame);

        UINT colorPixelCount = kinect->m_pWidth      * kinect->m_pHeight;
        UINT depthPixelCount = kinect->m_pDepthWidth * kinect->m_pDepthHeight;

        m_BaseCameraPointFrame = cv::Mat(kinect->m_pHeight, kinect->m_pWidth, CV_32FC3);
        HRESULT hr = kinect->m_pCoordinateMapper->MapColorFrameToCameraSpace(depthPixelCount, (UINT16*)kinect->m_depthMat.data, colorPixelCount, (CameraSpacePoint*)m_BaseCameraPointFrame.data);
        if (SUCCEEDED(hr))
        {
            m_CapturedBaseFrame = true;
        }

        return;
    }

    if (!m_PointPair)
    {
        std::vector<AJPointPairGenerator::PointPair> vectorList;

        for (int y = 0; y <= 10; ++y)
        {
            float yOffset = 0.9f - (static_cast<float>(y)* 0.08f);
            for (int x = 0; x <= 10; ++x)
            {
                float xOffset = 0.1f + (static_cast<float>(x)* 0.08f);
                AJPointPairGenerator::PointPair currentPair;
                currentPair.m_Source = XMVectorSet(xOffset, yOffset, 20.0f, 0.0f);
                currentPair.m_Result = XMVectorSet(-1.0f, -1.0f, 0.0f, 0.0f);
                currentPair.m_Found = false;
                vectorList.push_back(currentPair);
            }
        }
        m_PointPair = AJFactory::CreatePointPairObject(vectorList, m_BaseColorFrame);
        return;
    }
    
    AJPointPairComponent* ppComp = m_PointPair ? m_PointPair->FindComponent<AJPointPairComponent>() : nullptr;

    if (   ppComp
        && ppComp->GetStatus() == AJPointPairComponent::Status::Done)
    {
        if (m_FirstPlane.size() > 0)
        {
            // perform calibration
            AJKinectHelper::PerformCalibrationOnDelta(m_FirstPlane, ppComp->m_PointPair);


            AJColorCalibrationState* colorCalibrationState = new AJColorCalibrationState();
            AJFlowManager::GetInstance()->ReplaceState(colorCalibrationState);
        }
        else
        {
            FFProjectorCalibrationStateAdjustPlane* projectorCalibrationStateAdjustPlane = new FFProjectorCalibrationStateAdjustPlane(ppComp->m_PointPair);
            AJFlowManager::GetInstance()->ReplaceState(projectorCalibrationStateAdjustPlane);
        }
    }
}

cv::Mat AJProjectorCalibrationState::GetBaseColourFrame()
{
    return m_BaseColorFrame;
}

cv::Mat AJProjectorCalibrationState::GetBaseCameraPointFrame()
{
    return m_BaseCameraPointFrame;
}