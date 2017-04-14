#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

struct Pos
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct UV
{
    float u = 0.0f;
    float v = 0.0f;
};

bool IsSameFloat(float src, float other)
{
    float delta = fabs(src - other);
    return delta < 0.00005f;
}

bool IsSamePos(Pos& src, Pos& other)
{
    return IsSameFloat(src.x, other.x)
        && IsSameFloat(src.y, other.y)
        && IsSameFloat(src.z, other.z);
}

void main()
{
    vector<Pos> OriginalPos;
    vector<Pos> RemappedPos;
    vector<UV> RemappedUV;
    vector<UV> Resolved;

    int dataCount = 1347;

    OriginalPos.resize(dataCount);
    RemappedPos.resize(dataCount);
    RemappedUV.resize(dataCount);
    Resolved.resize(dataCount);

    ifstream originalPos;
    originalPos.open("faceModel_28.txt");
    if (originalPos.is_open())
    {
        for (int i = 0; i < dataCount; ++i)
        {
            float junk;
            Pos data;
            originalPos >> data.x >> data.y >> data.z >> junk;
            OriginalPos[i] = data;
        }
        originalPos.close();
    }

    ifstream remappedPos;
    remappedPos.open("Face_Unwrapped_v.txt");
    if (remappedPos.is_open())
    {
        for (int i = 0; i < dataCount; ++i)
        {
            Pos data;
            remappedPos >> data.x >> data.y >> data.z;
            RemappedPos[i] = data;
        }
        remappedPos.close();
    }

    ifstream remappedUV;
    remappedUV.open("Face_Unwrapped_uv.txt");
    if (remappedUV.is_open())
    {
        for (int i = 0; i < dataCount; ++i)
        {
            UV data;
            remappedUV >> data.u >> data.v;
            RemappedUV[i] = data;
        }
        remappedUV.close();
    }

    ofstream newFile;
    newFile.open("newData.txt");
    if (newFile.is_open())
    {
        for (int i = 0; i < dataCount; ++i)
        {
            bool found = false;
            Pos src = OriginalPos[i];
            for (int j = 0; j < dataCount; ++j)
            {
                if (IsSamePos(src, RemappedPos[j]))
                {
                    found = true;
                    Resolved[i] = RemappedUV[j];
                    break;
                }
            }

            if (!found)
            {
                int x = 10;
                int y = x + 10;
                y;
            }

            newFile << Resolved[i].u << " " << Resolved[i].v << endl;
        }
        newFile.close();
    }
}