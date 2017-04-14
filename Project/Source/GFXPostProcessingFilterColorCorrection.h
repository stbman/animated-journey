//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef AJ_GFX_POSTPROCESSING_FILTER_COLORCORRECTION_H
#define AJ_GFX_POSTPROCESSING_FILTER_COLORCORRECTION_H
//---------------------------------------------------------------------------

#include "../Source/GFXPostProcessingFilter.h"

struct CBColorCorrectionInfo
{
    float ColorMax[4]; // b, g, r, alpha

    CBColorCorrectionInfo();
};

class GFXPostProcessingFilterColorCorrection : public GFXPostProcessingFilter
{
private:
    ID3D11Buffer*         m_pCBColorCorrectionInfo = nullptr;
    CBColorCorrectionInfo m_CalibratedColor;
protected:
    virtual bool InitializeInternal();
public:
    ~GFXPostProcessingFilterColorCorrection();

    virtual void Update();
    void SetCalibratedColor(float b, float g, float r, float a);
};

#endif