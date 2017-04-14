//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_PROJECTOR_SEARCH_STATE_H
#define AJ_PROJECTOR_SEARCH_STATE_H
//---------------------------------------------------------------------------


#include "../Source/AJBaseState.h"
#include "../Source/gfxheader.h"
#include "AJFlowState.h"
#include <vector>

class AJObject;

class AJProjectorSearchState : public AJFlowState
{
private:
    std::vector<AJObject*> m_ObjList;
    float m_TimeLeft;

    AJObject* m_CalibrationCross[3];
    AJObject* m_PointPair = nullptr;

    std::vector<float> m_TransformationCoeff = std::vector<float>(12, 0.0);
    XMFLOAT3 m_ProjectorCenterVec;
    XMFLOAT3 m_ProjectorUpVec;
    XMFLOAT3 m_ProjectorLookAtVec;
    float m_ProjectorFOV;

    UINT m_FrameCounter = 0;
    bool m_CapturedBaseFrame = false;
    cv::Mat m_BaseColorFrame;
    cv::Mat m_BaseDepthFrame;
    cv::Mat m_BaseCameraPointFrame;

    void CalculateProjectorParameters();
    void CalculateProjectorCenterPos();
    void CalculateProjectorUpPos();
    void CalculateProjectorLookAt();

    void TransformationVecToMat();
public:
    AJProjectorSearchState();
    ~AJProjectorSearchState();

    virtual void OnEnter();
    virtual void OnExit();
    virtual void Update(float dt);

    void SolveForTransformationCoeff(std::vector<XMFLOAT3>KinectPoints,
        std::vector<XMFLOAT3>ProjectorPoints);
    XMFLOAT3 ConvertKinectToProjector(XMFLOAT3 KinectPoint);
    float SolveForProjectorFOV(XMFLOAT3 TopRight, XMFLOAT3 TopLeft,
        XMFLOAT3 BottomRight, XMFLOAT3 BottomLeft);

    cv::Mat GetBaseColourFrame();
    cv::Mat GetBaseDepthFrame();
    cv::Mat GetBaseCameraPointFrame();
};

#endif