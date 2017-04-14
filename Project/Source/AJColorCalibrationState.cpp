//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJColorCalibrationState.h"
#include "../Source/AJFactoryManager.h"
#include "../Source/AJObject.h"
#include "../Source/gfxHelper.h"
#include "../Source/AJFaceProjectionState.h"
#include "../Source/AJMathHelper.h"

AJColorCalibrationState::AJColorCalibrationState()
{
}

AJColorCalibrationState::~AJColorCalibrationState()
{
}

void AJColorCalibrationState::OnEnter()
{
    std::wstring texturepath = L"../Resource/ColorCalibration.dds";
    m_InfoText = AJFactory::CreateUIObject(texturepath, 0.75f);

    DirectX::XMVECTOR pos = GFXHelper::ConvertNormalizedToScreenSpace(0.5f, 0.5f, 0.0f);
    m_InfoText->SetGlobalPosition(pos);

    m_TimeLeft = 0.0f;
    m_CaptureStage = 0;
    m_GFXPostProcessingFilterColorCorrection = GFXPostProcessManager::GetInstance()->FindFilter<GFXPostProcessingFilterColorCorrection>();
    if (m_GFXPostProcessingFilterColorCorrection == nullptr)
    {
        m_GFXPostProcessingFilterColorCorrection = new GFXPostProcessingFilterColorCorrection();
        m_GFXPostProcessingFilterColorCorrection->SetupShaders(L"../Shader/GFXShaderColorCorrection.fx");
        m_GFXPostProcessingFilterColorCorrection->SetCalibratedColor(1.0f, 1.0f, 1.0f, 1.0f);
        GFXPostProcessManager::GetInstance()->AddPostProcessingFilter(m_GFXPostProcessingFilterColorCorrection);
    }
}

void AJColorCalibrationState::OnExit()
{
    for each(AJObject* obj in m_ObjList)
    {
        delete obj;
    }

    m_ObjList.clear();
}

void AJColorCalibrationState::Update(float dt)
{
    m_TimeLeft += dt;
    if (m_CaptureStage == 0)
    {
        if (m_TimeLeft >= 2.0f)
        {
            delete m_InfoText;
            m_InfoText = nullptr;
            m_CaptureStage = 1;

            GFXManager::GetInstance()->SetClearRed(0.0f);
            GFXManager::GetInstance()->SetClearGreen(0.0f);
            GFXManager::GetInstance()->SetClearBlue(0.0f);
            GFXManager::GetInstance()->SetClearAlpha(0.0f);
            //FFFaceProjectionState* faceProjectionState = new FFFaceProjectionState();
            //FFFlowManager::GetInstance()->ReplaceState(faceProjectionState);
        }
    }
    else if (m_CaptureStage == 1)
    {
        if (m_TimeLeft >= 4.0f)
        {
            m_CaptureStage = 2;
            GFXManager::GetInstance()->SetClearRed(1.0f);
            GFXManager::GetInstance()->SetClearGreen(1.0f);
            GFXManager::GetInstance()->SetClearBlue(1.0f);
            GFXManager::GetInstance()->SetClearAlpha(1.0f);
            // capture frame

            AJKinect::GetInstance()->m_colorMat.copyTo(m_BaseBlack);
        }
    }
    else if (m_CaptureStage == 2)
    {
        if (m_TimeLeft > 6.0f)
        {
            GFXManager::GetInstance()->SetClearRed(0.0f);
            GFXManager::GetInstance()->SetClearGreen(0.0f);
            GFXManager::GetInstance()->SetClearBlue(0.0f);
            GFXManager::GetInstance()->SetClearAlpha(0.0f);

            // capture frame
            AJKinect::GetInstance()->m_colorMat.copyTo(m_BaseWhite);

            // do calibration
            cv::Mat diff;
            cv::subtract(m_BaseWhite, m_BaseBlack, diff);

            int lowValue = 80, highValue = 255;
            cv::Mat threshold;
            cv::threshold(diff, threshold, lowValue, highValue, CV_THRESH_BINARY);

            cv::Mat canny;
            cv::Canny(threshold, canny, lowValue, lowValue * 2, 3);

            std::vector<std::vector<cv::Point>> contours;
            std::vector<cv::Vec4i> hierarchy;

            cv::findContours(canny, contours, hierarchy, CV_RETR_TREE,
                CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

            XMFLOAT2 centerPoint;
            double largest_area = 0;
            int largest_contour_index = 0;
            cv::Rect largestRect;
            if (contours.size() > 0)
            {
                for (int i = 0; i < contours.size(); i++)
                {
                    cv::Rect boundingRectForContour = cv::boundingRect(contours[i]);

                    // iterate through each contour. 
                    double a = boundingRectForContour.area();// contourArea(contours[i], false);  //  Find the area of contour
                    if (a > largest_area)
                    {
                        largest_area = a;
                        largest_contour_index = i;                //Store the index of largest contour
                        largestRect = cv::boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
                    }
                }

                // Calculate center of largest contour using moments 
                cv::Moments moment = cv::moments(contours[largest_contour_index]);
                centerPoint.x = (float)(moment.m10 / moment.m00);
                centerPoint.y = (float)(moment.m01 / moment.m00);
            }

            XMFLOAT3 color; // bgr
            AJKinectHelper::GetImageColorValue(color, diff, centerPoint);

            if (   color.x <= color.y
                && color.x <= color.z)
            {
                color.y = AJMathHelper::CursorizeAndClamp(color.x, 0.0f, color.y);
                color.z = AJMathHelper::CursorizeAndClamp(color.x, 0.0f, color.z);
                color.x = 1.0f;
            }
            else if (   color.y <= color.x
                     && color.y <= color.z)
            {
                color.x = AJMathHelper::CursorizeAndClamp(color.y, 0.0f, color.x);
                color.z = AJMathHelper::CursorizeAndClamp(color.y, 0.0f, color.z);
                color.y = 1.0f;
            }
            else
            {
                color.x = AJMathHelper::CursorizeAndClamp(color.z, 0.0f, color.x);
                color.y = AJMathHelper::CursorizeAndClamp(color.z, 0.0f, color.y);
                color.z = 1.0f;
            }
            m_GFXPostProcessingFilterColorCorrection->SetCalibratedColor(color.x, color.y, color.z, 1.0f);

            // done
            AJFaceProjectionState* faceProjectionState = new AJFaceProjectionState();
            AJFlowManager::GetInstance()->ReplaceState(faceProjectionState);
        }
    }
}
