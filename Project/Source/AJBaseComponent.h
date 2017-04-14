//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_BASECOMPONENT_H
#define AJ_BASECOMPONENT_H
//---------------------------------------------------------------------------

#include "../Source/AJObject.h"

class AJBaseComponent
{
private:
    // system
    bool m_Active = true;
public:
    AJObject* m_Owner = nullptr;
    AJObject* GetOwner() { return m_Owner; }
    bool GetActive() { return m_Active; }
    void SetActive(bool active) { m_Active = active; }

    virtual void Update() {}
};

#endif