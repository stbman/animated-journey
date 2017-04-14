#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct XMFLOAT2
{
    float x;
    float y;
};

struct XMFLOAT3
{
    float x;
    float y;
    float z;
};

struct SimpleVertex
{
    XMFLOAT3 Pos;
    XMFLOAT2 Tex;
    XMFLOAT3 Nor;
};

struct SimpleIndices
{
    unsigned int x;
    unsigned int y;
    unsigned int z;
};

void OutputBlock(SimpleVertex* array, unsigned int verticesSize, SimpleIndices* indices, unsigned int indicesCount, string filename)
{
    std::ofstream file;
    file.open(filename.c_str());
    for (unsigned int i = 0; i < verticesSize; ++i)
    {
        file << "v " << array[i].Pos.x << " " << array[i].Pos.y << " " << array[i].Pos.z << " 0.0" << endl;
    }

    file << endl;

    //for (unsigned int i = 0; i < verticesSize; ++i)
    //{
    //    file << "vt " << array[i].Tex.x << " " << array[i].Tex.y << " 0.0" << endl;
    //}

    for (unsigned int i = 0; i < indicesCount; ++i)
    {
        file << "f " << indices[i].x+1 << " " << indices[i].y+1 << " " << indices[i].z+1 << endl;
    }

    file.close();
}

void main(void)
{
    unsigned int indicesCount = 0;
    SimpleIndices* indicesBuffer = nullptr;

    std::ifstream indicesFile;
    indicesFile.open("../resources/indices.apf", std::ios::binary);
    if (indicesFile.is_open())
    {
        indicesFile.seekg(0, std::ios::end);
        unsigned int length = (unsigned int)indicesFile.tellg();
        indicesFile.seekg(0, std::ios::beg);
        indicesCount = length / sizeof(SimpleIndices);
        indicesBuffer = new SimpleIndices[indicesCount];
        // read data as a block:
        indicesFile.read((char*)indicesBuffer, length);
        indicesFile.close();

    }

    string fileBase = "../resources/lynnette/face";
    std::ifstream newFile;
    newFile.open((fileBase + ".apf").c_str(), std::ios::binary);
    if (newFile.is_open())
    {
        newFile.seekg(0, std::ios::end);
        unsigned int length = (unsigned int)newFile.tellg();
        newFile.seekg(0, std::ios::beg);
        unsigned int framelength = 1347;
        unsigned int frames = length / framelength;
        unsigned int vertexCount = length / sizeof(SimpleVertex);
        SimpleVertex* buffer = new SimpleVertex[vertexCount];
        // read data as a block:
        newFile.read((char*)buffer, length);
        newFile.close();

        --frames;
        for (unsigned int i = 0; i < frames; ++i)
        {
            string filepath = fileBase + to_string(i) + ".obj";
            OutputBlock(buffer + (framelength * i), framelength, indicesBuffer, indicesCount, filepath);
        }

        delete[] buffer;
        delete[] indicesBuffer;
    }
}