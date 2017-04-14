//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJPointPairLogicBaseState.h"

AJPointPairLogicBaseState::AJPointPairLogicBaseState(AJPointPairComponent* parent)
{
    m_Owner = parent;
}

AJPointPairLogicBaseState::~AJPointPairLogicBaseState()
{
}

AJPointPairComponent* AJPointPairLogicBaseState::GetOwner()
{
    return m_Owner;
}

AJPointPairLogicBaseState::Status AJPointPairLogicBaseState::GetStatus()
{
    return Status::Error;
}