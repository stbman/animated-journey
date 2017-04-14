//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_FACE_PROJECTION_STATE_H
#define AJ_FACE_PROJECTION_STATE_H
//---------------------------------------------------------------------------

#include "AJFlowState.h"
#include <vector>

class AJObject;

class AJFaceProjectionState : public AJFlowState
{
private:
    std::vector<AJObject*> m_ObjList;

public:
    AJFaceProjectionState();
    ~AJFaceProjectionState();

    virtual void OnEnter();
    virtual void OnExit();
    virtual void Update(float dt);
};

#endif