//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_POINT_PAIR_GENERATOR_H
#define AJ_POINT_PAIR_GENERATOR_H
//---------------------------------------------------------------------------

#include "..\Source\AJMathHeader.h"

class AJPointPairGenerator
{
private:
public:
    enum Status
    {
        Projecting,
        Searching,
        Done,
        Failure
    };

    struct PointPair
    {
        DirectX::XMVECTOR m_Result;
        DirectX::XMVECTOR m_Source;
        bool              m_Found;
    };

    AJPointPairGenerator();
    ~AJPointPairGenerator();

    Status GetStatus();
    void Init(DirectX::XMVECTOR source);
    void Update();
    PointPair GetCurrentResult();
};

#endif