//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_GFXHEADER_H
#define AJ_GFXHEADER_H
//---------------------------------------------------------------------------

// directx resources
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include "../Library/DDSTextureLoader.h"
#include "../Resource/resource.h"

//  Includes
#include "../Source/AJMathHeader.h"
#include "../Source/gfxmanager.h"
#include "../Source/AJKinect.h"
#include "../Source/GFXPostProcessManager.h"

#include <AntTweakBar.h>

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
    XMFLOAT3 Pos;
    XMFLOAT2 Tex;
    XMFLOAT3 Nor;
};

struct CBViewMatrix
{
    XMMATRIX mView;
};

struct CBProjectionMatrix
{
    XMMATRIX mProjection;
};

struct CBWorldMatrix
{
    XMMATRIX mWorld;
};

#endif