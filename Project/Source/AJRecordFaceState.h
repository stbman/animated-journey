//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_RECORD_FACE_STATE_H
#define AJ_RECORD_FACE_STATE_H
//---------------------------------------------------------------------------

#include "AJFlowState.h"
#include <vector>
#include "../Source/gfxHelper.h"

class AJObject;

class AJRecordFaceState : public AJFlowState
{
private:
    std::vector<AJObject*> m_ObjList;
    AJObject* m_StartRecording = nullptr;
    AJObject* m_StopRecording = nullptr;
public:
    AJRecordFaceState();
    ~AJRecordFaceState();

    TwBar* m_DemoMenu      = nullptr;
    bool   m_ShowDemoMenu  = true;

    virtual void OnEnter();
    virtual void OnExit();
    virtual void Update(float dt);
};

#endif