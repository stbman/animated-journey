//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_POINTPAIR_LOGIC_BASE_STATE_H
#define AJ_POINTPAIR_LOGIC_BASE_STATE_H
//---------------------------------------------------------------------------

#include "../Source/AJBaseState.h"
#include "../Source/AJPointPairComponent.h"

class AJPointPairLogicBaseState: public AJBaseState
{
private:
    AJPointPairComponent* m_Owner = nullptr;

public:
    enum Status
    {
        Error,
        InProgress,
        Done
    };

    AJPointPairLogicBaseState(AJPointPairComponent* parent);
    ~AJPointPairLogicBaseState();

    AJPointPairComponent* GetOwner();
    virtual AJPointPairLogicBaseState::Status GetStatus();
};

#endif