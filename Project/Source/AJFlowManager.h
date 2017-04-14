//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_FLOWMANAGER_H
#define AJ_FLOWMANAGER_H
//---------------------------------------------------------------------------

#include "../Source/AJStateManager.h"

class AJFlowState;

class AJFlowManager
{
private:
    AJFlowManager();
    ~AJFlowManager();

    AJStateManager m_StateManager;

public:
    static AJFlowManager* GetInstance();

    void PushState(AJFlowState* state);
    void DeleteState(AJFlowState* state);
    void PopState();
    void ReplaceState(AJFlowState* state);
    void ClearStates();
    AJBaseState* GetCurrentState();

    void Update(float dt);
};

#endif