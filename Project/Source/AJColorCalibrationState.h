//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_COLOR_CALIBRATION_STATE_H
#define AJ_COLOR_CALIBRATION_STATE_H
//---------------------------------------------------------------------------

#include "AJFlowState.h"
#include "../Source/AJKinectHelper.h"
#include "../Source/GFXPostProcessingFilterColorCorrection.h"
#include <vector>

class AJObject;

class AJColorCalibrationState : public AJFlowState
{
private:
    std::vector<AJObject*> m_ObjList;
    AJObject* m_InfoText;

    float m_TimeLeft;
    int  m_CaptureStage;

    cv::Mat m_BaseBlack;
    cv::Mat m_BaseWhite;

    GFXPostProcessingFilterColorCorrection* m_GFXPostProcessingFilterColorCorrection;
public:
    AJColorCalibrationState();
    ~AJColorCalibrationState();

    virtual void OnEnter();
    virtual void OnExit();
    virtual void Update(float dt);
};

#endif