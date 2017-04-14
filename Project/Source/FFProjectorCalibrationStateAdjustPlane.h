//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_PROJECTOR_CALIBRATION_STATE_ADJUST_PLANE_H
#define AJ_PROJECTOR_CALIBRATION_STATE_ADJUST_PLANE_H
//---------------------------------------------------------------------------


#include "../Source/AJBaseState.h"
#include "../Source/AJFlowState.h"
#include "../Source/AJPointPairGenerator.h"

class AJObject;

class FFProjectorCalibrationStateAdjustPlane : public AJFlowState
{
private:
    std::vector<AJObject*> m_ObjList;

    std::vector<AJPointPairGenerator::PointPair> m_FirstPlane;
public:
    FFProjectorCalibrationStateAdjustPlane(std::vector<AJPointPairGenerator::PointPair>& vectorList);
    ~FFProjectorCalibrationStateAdjustPlane();

    virtual void OnEnter();
    virtual void OnExit();
    virtual void Update(float dt);
};

#endif