//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_POINTPAIR_LOGIC_BASE_STATE_INIT_H
#define AJ_POINTPAIR_LOGIC_BASE_STATE_INIT_H
//---------------------------------------------------------------------------

#include "../Source/AJPointPairLogicBaseState.h"

class AJPointPairLogicInit : public AJPointPairLogicBaseState
{
private:
    DirectX::XMVECTOR m_SourcePosition;
    bool              m_SetPosition = false;
    int count = 0;

public:
    AJPointPairLogicInit(AJPointPairComponent* parent, DirectX::XMVECTOR position);
    ~AJPointPairLogicInit();

    virtual void OnEnter();
    virtual void OnExit();
    virtual void Update(float dt);

    virtual AJPointPairLogicBaseState::Status GetStatus();
};

#endif