//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef AJ_GFXORTHOCAMERA_H
#define AJ_GFXORTHOCAMERA_H
//---------------------------------------------------------------------------

#include "../Source/predefine.h"
#include "../Source/gfxbasecamera.h"
#include <DirectXMath.h>

class GFXOrthoCamera : public GFXBaseCamera
{
private:
    struct OrthoCameraVector
    {
        float m_Up    = 300.0f;
        float m_Right = 400.0f;
    };

    OrthoCameraVector m_CameraHalfVector;
    OrthoCameraVector m_CameraCenter;

public:
    GFXOrthoCamera();

    void SetCameraHalfVector(float up, float right);
    void SetCameraCenter(float up, float right);

    float GetCameraHalfVectorUp();
    float GetCameraHalfVectorRight();
    float GetCameraCenterUp();
    float GetCameraCenterRight();

    void virtual Update();
};

#endif