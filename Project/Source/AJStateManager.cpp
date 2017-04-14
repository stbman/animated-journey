//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJStateManager.h"
#include "../Source/AJBaseState.h"

#include <algorithm>

AJStateManager::AJStateManager()
{
}

AJStateManager::~AJStateManager()
{
    ClearStates();
}

void AJStateManager::AddState(AJBaseState* state)
{
    m_States.push_front(state);
    state->OnEnter();
}

void AJStateManager::RemoveState(AJBaseState* state)
{
    state->OnExit();
    m_States.remove(state);
    delete state;
}

void AJStateManager::StateManagementUpdate()
{
    for each(AJBaseState* state in m_PopState)
    {
        RemoveState(state);
    }
    m_PopState.clear();

    for each(AJBaseState* state in m_PushState)
    {
        AddState(state);
    }
    m_PushState.clear();
}

void AJStateManager::PushState(AJBaseState* state)
{
    if (!state)
    {
        return;
    }

    bool found = (std::find(m_States.begin(), m_States.end(), state) != m_States.end());
    if (!found)
    {
        m_PushState.push_back(state);
    }
}

void AJStateManager::DeleteState(AJBaseState* state)
{
    if (!state)
    {
        return;
    }

    bool found = (std::find(m_States.begin(), m_States.end(), state) != m_States.end());
    if (found)
    {
        m_PopState.push_back(state);
    }
}

void AJStateManager::PopState()
{
    if (!m_States.empty())
    {
        m_PopState.push_back(m_States.front());
    }
}

void AJStateManager::ReplaceState(AJBaseState* state)
{
    PopState();
    PushState(state);
}

void AJStateManager::ClearStates()
{
    // to flush all pending push states
    StateManagementUpdate();

    for each(AJBaseState* state in m_States)
    {
        DeleteState(state);
    }

    // to flush all pending pop states
    StateManagementUpdate();
}

void AJStateManager::Update(float dt)
{
    StateManagementUpdate();

    for each(AJBaseState* state in m_States)
    {
        state->Update(dt);
    }
}

AJBaseState* AJStateManager::GetCurrentState()
{
    return !m_States.empty() ? m_States.front() : nullptr;
}