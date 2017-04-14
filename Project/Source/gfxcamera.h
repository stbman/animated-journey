//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef AJ_GFXCAMERA_H
#define AJ_GFXCAMERA_H
//---------------------------------------------------------------------------

#include "../Source/gfxbasecamera.h"
#include <DirectXMath.h>
#include "../Source/AJMathHelper.h"

class GFXCamera : public GFXBaseCamera
{
private:

    float m_FOV         = AJMathHelper::DegreeToRadian(20.0f);
    float m_AspectRatio = 0.0f;

public:
    GFXCamera();

    float GetFOV();
    float GetAspectRatio();
    void SetFOV(float fov);
    void SetAspectRatio(float aspectRatio);
    void virtual Update();
};

#endif