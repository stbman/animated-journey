//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJPointPairLogicInit.h"
#include "../Source/gfxHelper.h"

AJPointPairLogicInit::AJPointPairLogicInit(AJPointPairComponent* parent, DirectX::XMVECTOR position) : AJPointPairLogicBaseState(parent)
{
    m_SourcePosition = position;
}

AJPointPairLogicInit::~AJPointPairLogicInit()
{
}

void AJPointPairLogicInit::OnEnter()
{
    // update position of the point
    GetOwner()->GetOwner()->SetGlobalPosition(GFXHelper::ConvertNormalizedToScreenSpace(XMVectorGetX(m_SourcePosition), XMVectorGetY(m_SourcePosition), XMVectorGetZ(m_SourcePosition)));
    m_SetPosition = true;
}

void AJPointPairLogicInit::OnExit()
{

}

void AJPointPairLogicInit::Update(float dt)
{
    dt;
}

AJPointPairLogicBaseState::Status AJPointPairLogicInit::GetStatus()
{
    return m_SetPosition ? AJPointPairLogicBaseState::Status::Done : AJPointPairLogicBaseState::Status::InProgress;
}