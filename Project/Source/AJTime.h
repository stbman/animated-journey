//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_TIME_H
#define AJ_TIME_H
//---------------------------------------------------------------------------

#include <windows.h>

class AJTime
{
private:
    ULONGLONG m_CurrentTime = 0;
    ULONGLONG m_DeltaTime   = 0;
    float     m_Delta       = 0.0f;
    float     m_FPS         = 0.0f;
public:
    void Start();
    float Update();

    ULONGLONG GetCurrentTime();
    ULONGLONG GetActualDeltaTime();
    float GetDeltaTime();
    float GetFPS();
};

#endif