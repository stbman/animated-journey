//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/gfxorthocamera.h"

GFXOrthoCamera::GFXOrthoCamera() : GFXBaseCamera()
{
    OrthoCameraVector cameraHalf;
    cameraHalf.m_Up    = static_cast<float>(m_GFXManagerInstance->GetWindowHeight()) * 0.5f;
    cameraHalf.m_Right = static_cast<float>(m_GFXManagerInstance->GetWindowWidth()) * 0.5f;
    m_CameraHalfVector = cameraHalf;
    m_CameraCenter     = cameraHalf;

    m_CameraPos    = XMVectorSet(0.0f, 0.0f, -10.0f, 0.0f);
    m_CameraLookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    m_CameraUpVec  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
}

void GFXOrthoCamera::SetCameraHalfVector(float up, float right)
{
    m_CameraHalfVector.m_Up    = up;
    m_CameraHalfVector.m_Right = right;
}

void GFXOrthoCamera::SetCameraCenter(float up, float right)
{
    m_CameraCenter.m_Up = up;
    m_CameraCenter.m_Right = right;
}

float GFXOrthoCamera::GetCameraHalfVectorUp()
{
    return m_CameraHalfVector.m_Up;
}

float GFXOrthoCamera::GetCameraHalfVectorRight()
{
    return m_CameraHalfVector.m_Right;
}

float GFXOrthoCamera::GetCameraCenterUp()
{
    return m_CameraCenter.m_Up;
}

float GFXOrthoCamera::GetCameraCenterRight()
{
    return m_CameraCenter.m_Right;
}

void GFXOrthoCamera::Update()
{
    m_View       = XMMatrixLookAtLH(m_CameraPos, m_CameraLookAt, m_CameraUpVec);
    m_Projection = XMMatrixOrthographicOffCenterLH(m_CameraCenter.m_Right - m_CameraHalfVector.m_Right
                                                 , m_CameraCenter.m_Right + m_CameraHalfVector.m_Right
                                                 , m_CameraCenter.m_Up    - m_CameraHalfVector.m_Up
                                                 , m_CameraCenter.m_Up    + m_CameraHalfVector.m_Up
                                                 , m_NearClipPlane
                                                 , m_FarClipPlane);
}