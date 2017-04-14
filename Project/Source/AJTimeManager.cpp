//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJTimeManager.h"

AJTimeManager::AJTimeManager()
{
}

AJTimeManager::~AJTimeManager()
{
}

AJTimeManager* AJTimeManager::GetInstance()
{
    static AJTimeManager m_Instance;
    return &m_Instance;
}

void AJTimeManager::Start()
{
    m_Time.Start();
}

float AJTimeManager::Update()
{
    return m_Time.Update();
}

ULONGLONG AJTimeManager::GetCurrentTime()
{
    return m_Time.GetCurrentTime();
}

ULONGLONG AJTimeManager::GetActualDeltaTime()
{
    return m_Time.GetActualDeltaTime();
}

float AJTimeManager::GetDeltaTime()
{
    return m_Time.GetDeltaTime();
}

float AJTimeManager::GetFPS()
{
    return m_Time.GetFPS();
}