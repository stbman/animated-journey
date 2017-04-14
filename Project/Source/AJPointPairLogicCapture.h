//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_POINTPAIR_LOGIC_BASE_STATE_CAPTURE_H
#define AJ_POINTPAIR_LOGIC_BASE_STATE_CAPTURE_H
//---------------------------------------------------------------------------

#include "../Source/AJPointPairLogicBaseState.h"
#include "../Source/AJProjectorSearchState.h"
#include <opencv2/opencv.hpp>

class AJPointPairLogicCapture : public AJPointPairLogicBaseState
{
private:
    DirectX::XMVECTOR m_Result;
    float             m_ScanTimer = 0.0f;

    UINT m_FrameCounter = 0;
    bool m_FoundPoint = false;

    cv::Mat           m_BaseColourMat;

    static UINT m_CurrentImage;

    void SaveImage(std::string path, cv::Mat image);

public:
    AJPointPairLogicCapture(AJPointPairComponent* parent);
    ~AJPointPairLogicCapture();

    virtual void OnEnter();
    virtual void OnExit();
    virtual void Update(float dt);

    virtual AJPointPairLogicBaseState::Status GetStatus();
    DirectX::XMVECTOR GetResult();
};

#endif