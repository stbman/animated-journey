//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/GFXPostProcessingFilterColorCorrection.h"

CBColorCorrectionInfo::CBColorCorrectionInfo()
{
    ColorMax[0] = 1.0f;
    ColorMax[1] = 1.0f;
    ColorMax[2] = 1.0f;
    ColorMax[3] = 1.0f;
}

GFXPostProcessingFilterColorCorrection::~GFXPostProcessingFilterColorCorrection()
{
    //constant buffer
    SafeRelease(m_pCBColorCorrectionInfo);
}

bool GFXPostProcessingFilterColorCorrection::InitializeInternal()
{
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CBColorCorrectionInfo);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HRESULT hr = GFXManager::GetInstance()->GetD3DDevice()->CreateBuffer(&bd, nullptr, &m_pCBColorCorrectionInfo);
    if (FAILED(hr))
    {
        return false;
    }

    GFXManager::GetInstance()->GetD3DDeviceContext()->UpdateSubresource(m_pCBColorCorrectionInfo, 0, nullptr, &m_CalibratedColor, 0, 0);
    return true;
}

void GFXPostProcessingFilterColorCorrection::Update()
{
    ID3D11DeviceContext* context = GFXManager::GetInstance()->GetD3DDeviceContext();
    context->UpdateSubresource(m_pCBColorCorrectionInfo, 0, nullptr, &m_CalibratedColor, 0, 0);
    context->VSSetShader(GetVertexShader(), nullptr, 0);
    context->VSSetConstantBuffers(3, 1, &m_pCBColorCorrectionInfo);
    context->PSSetShader(GetPixelShader(), nullptr, 0);
    context->PSSetConstantBuffers(3, 1, &m_pCBColorCorrectionInfo);
}

void GFXPostProcessingFilterColorCorrection::SetCalibratedColor(float b, float g, float r, float a)
{
    m_CalibratedColor.ColorMax[0] = b;
    m_CalibratedColor.ColorMax[1] = g;
    m_CalibratedColor.ColorMax[2] = r;
    m_CalibratedColor.ColorMax[3] = a;
}