//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_STATEMANAGER_H
#define AJ_STATEMANAGER_H
//---------------------------------------------------------------------------

#include <list>

class AJBaseState;

class AJStateManager
{
private:
    std::list<AJBaseState*> m_States;
    std::list<AJBaseState*> m_PushState;
    std::list<AJBaseState*> m_PopState;

    void AddState(AJBaseState* state);
    void RemoveState(AJBaseState* state);
    void StateManagementUpdate();
public:
    AJStateManager();
    ~AJStateManager();

    void PushState(AJBaseState* state);
    void DeleteState(AJBaseState* state);
    void PopState();
    void ReplaceState(AJBaseState* state);
    void ClearStates();
    AJBaseState* GetCurrentState();

    void Update(float dt);
    // pop state
    // replace
};

#endif