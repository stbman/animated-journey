//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef AJ_GFX_BASE_CAMERA_H
#define AJ_GFX_BASE_CAMERA_H
//---------------------------------------------------------------------------

#include "..\Source\AJMathHeader.h"

class GFXManager;

class GFXBaseCamera
{
protected:
    DirectX::XMMATRIX m_View       = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX m_Projection = DirectX::XMMatrixIdentity();

    XMVECTOR m_CameraPos    = XMVectorSet( 3.0f,  0.0f, -1.0f, 0.0f);
    XMVECTOR m_CameraLookAt = XMVectorSet( 0.0f,  0.0f,  1.0f, 0.0f);
    XMVECTOR m_CameraUpVec  = XMVectorSet( 0.0f,  1.0f,  0.0f, 0.0f);

    float m_NearClipPlane = 0.01f;
    float m_FarClipPlane = 100.0f;

    /// cache
    GFXManager* m_GFXManagerInstance = nullptr;

public:
    GFXBaseCamera();
    const DirectX::XMMATRIX& GetViewMatrix();
    const DirectX::XMMATRIX& GetProjMatrix();

    float GetNearClipPlane();
    float GetFarClipPlane();
    
    XMVECTOR GetCameraPosition();
    XMVECTOR GetCameraLookAt();
    XMVECTOR GetCameraUpVector();
    void SetCameraPosition(XMVECTOR& camPos);
    void SetCameraLookAt(XMVECTOR& lookAt);
    void SetCameraUpVector(XMVECTOR& upVec);

    void Init();
    void virtual Update() = 0;
};

#endif