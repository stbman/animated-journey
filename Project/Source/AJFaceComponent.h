//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_FACE_COMPONENT_H
#define AJ_FACE_COMPONENT_H
//---------------------------------------------------------------------------

#include "../Source/AJBaseComponent.h"

class AJKinect;
struct SimpleVertex;

typedef unsigned short WORD;

class AJFaceComponent : public AJBaseComponent
{
private:
    AJKinect* m_KinectCache = nullptr;
    int       m_Index       = -1;

    SimpleVertex* m_vertexData = nullptr;
    WORD* m_newIndicesData     = nullptr;

public:
    AJFaceComponent();
    void virtual Update();
    void Init(int index);

    void SaveIndices();
    bool IsTracked();

    const SimpleVertex* GetVertexData() { return m_vertexData; }
};

#endif
