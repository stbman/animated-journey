//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/predefine.h"
#include "../Source/gfxcamera.h"

GFXCamera::GFXCamera() : GFXBaseCamera()
{
    m_AspectRatio  = static_cast<float>(m_GFXManagerInstance->GetWindowWidth()) / static_cast<float>(m_GFXManagerInstance->GetWindowHeight());
    m_CameraPos    = XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f);
    m_CameraLookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    m_CameraUpVec  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    GFXManager* gfxManager = GFXManager::GetInstance();
    m_FOV = ((float)gfxManager->GetWindowWidth()) / ((float)gfxManager->GetWindowHeight());
}

float GFXCamera::GetFOV()
{
    return m_FOV;
}

float GFXCamera::GetAspectRatio()
{
    return m_AspectRatio;
}

void GFXCamera::SetFOV(float fov)
{
    m_FOV = fov;
}

void GFXCamera::SetAspectRatio(float aspectRatio)
{
    m_AspectRatio = aspectRatio;
}

void GFXCamera::Update()
{
    m_View       = XMMatrixLookAtLH(m_CameraPos, m_CameraLookAt, m_CameraUpVec);
    m_Projection = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, m_NearClipPlane, m_FarClipPlane);
}