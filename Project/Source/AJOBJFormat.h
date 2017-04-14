//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_OBJ_FORMAT_STATE_H
#define AJ_OBJ_FORMAT_STATE_H
//---------------------------------------------------------------------------

#include <string>
#include <vector>
#include <fstream>

class AJObject;
struct SimpleVertex;

class AJOBJFormat
{
private:
    std::vector<SimpleVertex> m_Data;
    std::vector<SimpleVertex> m_LastData;
    std::fstream m_File;
    
public:
    AJOBJFormat();
    ~AJOBJFormat();

    std::vector<AJObject*> m_ObjList;

    void Init(std::wstring filePath);
    void WriteBuffer(const SimpleVertex* buffer, unsigned size);
    void Close();
};

#endif