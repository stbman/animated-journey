//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_PROJECTOR_CALIBRATION_STATE_H
#define AJ_PROJECTOR_CALIBRATION_STATE_H
//---------------------------------------------------------------------------


#include "../Source/AJBaseState.h"
#include "../Source/gfxheader.h"
#include "../Source/AJPointPairComponent.h"
#include "../Source/AJFlowState.h"
#include <vector>

class AJObject;

class AJProjectorCalibrationState : public AJFlowState
{
private:
    std::vector<AJObject*> m_ObjList;

    AJObject* m_PointPair = nullptr;

    std::vector<float> m_TransformationCoeff = std::vector<float>(12, 0.0);
    XMFLOAT3 m_ProjectorCenterVec;
    XMFLOAT3 m_ProjectorUpVec;
    XMFLOAT3 m_ProjectorLookAtVec;
    float m_ProjectorFOV;

    UINT m_FrameCounter = 0;
    bool m_CapturedBaseFrame = false;
    cv::Mat m_BaseColorFrame;
    cv::Mat m_BaseCameraPointFrame;

    std::vector<AJPointPairGenerator::PointPair> m_FirstPlane;

public:
    AJProjectorCalibrationState();
    AJProjectorCalibrationState(std::vector<AJPointPairGenerator::PointPair>& vectorList);
    ~AJProjectorCalibrationState();

    virtual void OnEnter();
    virtual void OnExit();
    virtual void Update(float dt);

    cv::Mat GetBaseColourFrame();
    cv::Mat GetBaseCameraPointFrame();
};

#endif