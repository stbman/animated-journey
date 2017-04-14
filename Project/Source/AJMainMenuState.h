//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_MAIN_MENU_STATE_H
#define AJ_MAIN_MENU_STATE_H
//---------------------------------------------------------------------------

#include "AJFlowState.h"
#include <vector>
#include "../Source/gfxHelper.h"

class AJObject;

class AJMainMenuState : public AJFlowState
{
private:
    std::vector<AJObject*> m_ObjList;

    TwBar *m_SelectionMenu = nullptr;

public:
    AJMainMenuState();
    ~AJMainMenuState();

    virtual void OnEnter();
    virtual void OnExit();
    virtual void Update(float dt);
};

#endif