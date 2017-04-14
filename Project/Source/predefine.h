//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef PRE_DEFINE_H
#define PRE_DEFINE_H
//---------------------------------------------------------------------------

// shared resources
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

// core header
#include "../Source/FFObjectManager.h"
#include "../Source/gfxheader.h"
#include "../Source/AJTimeManager.h"
#include "../Source/AJFlowManager.h"

template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
    if (pInterfaceToRelease != NULL){
        pInterfaceToRelease->Release();
        pInterfaceToRelease = NULL;
    }
}

template<class Interface>
inline void SafeDelete(Interface *& pInterfaceToRelease)
{
    if (pInterfaceToRelease != NULL){
        delete pInterfaceToRelease;
        pInterfaceToRelease = NULL;
    }
}

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Render();

#endif