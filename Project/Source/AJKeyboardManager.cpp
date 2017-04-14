//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJKeyboardManager.h"

AJKeyboardManager::AJKeyboardManager()
{
    ZeroMemory(m_CurrKey, sizeof(bool) * 256);
    ZeroMemory(m_PrevKey, sizeof(bool) * 256);
}

AJKeyboardManager::~AJKeyboardManager()
{
}

AJKeyboardManager* AJKeyboardManager::GetInstance()
{
    static AJKeyboardManager m_Instance;
    return &m_Instance;
}

bool AJKeyboardManager::Pressed(const Key key)
{
    return m_CurrKey[key];
}

bool AJKeyboardManager::Triggered(const Key key)
{
    return m_CurrKey[key] && !m_PrevKey[key];
}

bool AJKeyboardManager::Released(const Key key)
{
    return !m_CurrKey[key] && m_PrevKey[key];
}

void AJKeyboardManager::Update()
{
    for (UINT i = 0; i < Key::TOTAL_KEYS; ++i)
    {
        m_PrevKey[i] = m_CurrKey[i];
        m_CurrKey[i] = (bool)GetAsyncKeyState(i);
    }
}