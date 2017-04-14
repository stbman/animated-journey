//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJPointPairGenerator.h"

AJPointPairGenerator::AJPointPairGenerator()
{
}

AJPointPairGenerator::~AJPointPairGenerator()
{
}

AJPointPairGenerator::Status AJPointPairGenerator::GetStatus()
{
    return AJPointPairGenerator::Failure;
}

void AJPointPairGenerator::Init(DirectX::XMVECTOR source)
{
    source;
}

void AJPointPairGenerator::Update()
{

}

AJPointPairGenerator::PointPair AJPointPairGenerator::GetCurrentResult()
{
    PointPair result;
    result.m_Result = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    result.m_Source = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    return result;
}