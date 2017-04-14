//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJTime.h"

void AJTime::Start()
{
    m_CurrentTime = GetTickCount64();
    m_DeltaTime   = 0;
    m_Delta       = 0.0f;
    m_FPS         = 0.0f;
}

float AJTime::Update()
{
    ULONGLONG currentTime = GetTickCount64();
    m_DeltaTime           = currentTime - m_CurrentTime;
    m_CurrentTime         = currentTime;
    m_Delta               = static_cast<float>(m_DeltaTime) * 0.001f;
    m_FPS                 = 1.0f / m_Delta;
    return m_Delta;
}

ULONGLONG AJTime::GetCurrentTime()
{
    return m_CurrentTime;
}

ULONGLONG AJTime::GetActualDeltaTime()
{
    return m_DeltaTime;
}

float AJTime::GetDeltaTime()
{
    return m_Delta;
}

float AJTime::GetFPS()
{
    return m_FPS;
}
