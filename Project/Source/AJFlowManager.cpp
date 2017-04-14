//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJFlowManager.h"
#include "../Source/AJFlowState.h"

AJFlowManager::AJFlowManager()
{
}

AJFlowManager::~AJFlowManager()
{
}

AJFlowManager* AJFlowManager::GetInstance()
{
    static AJFlowManager m_Instance;
    return &m_Instance;
}

void AJFlowManager::PushState(AJFlowState* state)
{
    m_StateManager.PushState(state);
}

void AJFlowManager::DeleteState(AJFlowState* state)
{
    m_StateManager.DeleteState(state);
}

void AJFlowManager::PopState()
{
    m_StateManager.PopState();
}

void AJFlowManager::ReplaceState(AJFlowState* state)
{
    m_StateManager.ReplaceState(state);
}

void AJFlowManager::ClearStates()
{
    m_StateManager.ClearStates();
}

void AJFlowManager::Update(float dt)
{
    m_StateManager.Update(dt);
}

AJBaseState* AJFlowManager::GetCurrentState()
{
    return m_StateManager.GetCurrentState();
}