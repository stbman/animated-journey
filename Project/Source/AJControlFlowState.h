//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_CONTROL_FLOW_STATE_H
#define AJ_CONTROL_FLOW_STATE_H
//---------------------------------------------------------------------------

#include "AJFlowState.h"

class GFXCamera;
class GFXOrthoCamera;

class AJControlFlowState : public AJFlowState
{
private:
    GFXCamera*      m_MainCamera = nullptr;
    GFXOrthoCamera* m_UICamera   = nullptr;
public:
    AJControlFlowState();
    ~AJControlFlowState();

    virtual void OnEnter();
    virtual void OnExit();
    virtual void Update(float dt);
};

#endif