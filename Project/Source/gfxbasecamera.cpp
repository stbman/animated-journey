//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/gfxmanager.h"
#include "../Source/gfxbasecamera.h"

GFXBaseCamera::GFXBaseCamera()
{
    m_GFXManagerInstance = GFXManager::GetInstance();
}

float GFXBaseCamera::GetNearClipPlane()
{
    return m_NearClipPlane;
}

float GFXBaseCamera::GetFarClipPlane()
{
    return m_FarClipPlane;
}

void GFXBaseCamera::Init()
{
}

XMVECTOR GFXBaseCamera::GetCameraPosition()
{
    return m_CameraPos;
}

XMVECTOR GFXBaseCamera::GetCameraLookAt()
{
    return m_CameraLookAt;
}

XMVECTOR GFXBaseCamera::GetCameraUpVector()
{
    return m_CameraUpVec;
}

void GFXBaseCamera::SetCameraPosition(XMVECTOR& camPos)
{
    m_CameraPos = camPos;
}

void GFXBaseCamera::SetCameraLookAt(XMVECTOR& lookAt)
{
    m_CameraLookAt = lookAt;
}

void GFXBaseCamera::SetCameraUpVector(XMVECTOR& upVec)
{
    m_CameraUpVec = upVec;
}

const DirectX::XMMATRIX& GFXBaseCamera::GetViewMatrix()
{
    return m_View;
}

const DirectX::XMMATRIX& GFXBaseCamera::GetProjMatrix()
{
    return m_Projection;
}