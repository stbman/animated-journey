//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_BASE_STATE_H
#define AJ_BASE_STATE_H
//---------------------------------------------------------------------------

class AJBaseState
{
private:

public:
    AJBaseState();
    ~AJBaseState();

    virtual void OnEnter() {}
    virtual void OnExit() {}
    virtual void Update(float dt) { dt; }
};

#endif