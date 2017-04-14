//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_TIME_MANAGER_H
#define AJ_TIME_MANAGER_H
//---------------------------------------------------------------------------

#include "../Source/AJTime.h"

class AJTimeManager
{
private:
    AJTimeManager();
    ~AJTimeManager();

    AJTime m_Time;
public:
    static AJTimeManager* GetInstance();

    void  Start();
    float Update();

    ULONGLONG GetCurrentTime();
    ULONGLONG GetActualDeltaTime();
    float GetDeltaTime();
    float GetFPS();
};

#endif
