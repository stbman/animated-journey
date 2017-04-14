#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Vertex
{
    float x;
    float y;
    float z;
};

struct Texture
{
    float u;
    float v;
};

struct Normal
{
    float x;
    float y;
    float z;
};

struct Indices
{
    int a;
    int b;
    int c;
};

struct FatIndices
{
    Indices a;
    Indices b;
    Indices c;
};

struct APFVertex
{
    Vertex Pos;
    Texture Tex;
    Normal Nor;
};

void main(void)
{
    string username = "Adrian";
    string basePath = "../Resource/" + username + "/";
    string originalPath = basePath + "original.obj";
    string edittedPath = basePath + "editted.obj";
    string resultPath = basePath + "anchor.apf";
    string indicesPath = basePath + "indices.apf";

    vector<Vertex>  originalFileVertex;
    vector<Texture> originalFileTexture;
    vector<Normal>  originalFileNormal;
    vector<Indices> originalFileIndices;
    vector<Vertex>  edittedFileVertex;
    vector<Texture> edittedFileTexture;
    vector<Normal>  edittedFileNormal;
    vector<FatIndices> edittedFileIndices;

    std::ifstream originalFile;
    originalFile.open(originalPath.c_str());
    if (originalFile.is_open())
    {
        string token;
        while (originalFile >> token)
        {
            if (token == "v")
            {
                Vertex data;
                float junk;
                originalFile >> data.x >> data.y >> data.z >> junk;
                originalFileVertex.push_back(data);
            }
            else if (token == "vt")
            {
                Texture data;
                originalFile >> data.u >> data.v;
                originalFileTexture.push_back(data);
            }
            else if (token == "vn")
            {
                Normal data;
                originalFile >> data.x >> data.y >> data.z;
                originalFileNormal.push_back(data);
            }
            else if (token == "f")
            {
                Indices data;
                originalFile >> data.a >> data.b >> data.c;
                originalFileIndices.push_back(data);
            }
            else
            {
                int x = 0;
            }
        }
        originalFile.close();
    }

    std::ifstream edittedFile;
    edittedFile.open(edittedPath.c_str());
    if (edittedFile.is_open())
    {
        string token;
        while (edittedFile >> token)
        {
            if (token == "v")
            {
                Vertex data;
                edittedFile >> data.x >> data.y >> data.z;
                edittedFileVertex.push_back(data);
            }
            else if (token == "vt")
            {
                Texture data;
                edittedFile >> data.u >> data.v;
                edittedFileTexture.push_back(data);
            }
            else if (token == "vn")
            {
                Normal data;
                edittedFile >> data.x >> data.y >> data.z;
                edittedFileNormal.push_back(data);
            }
            else if (token == "f")
            {
                string a, b, c;
                edittedFile >> a >> b >> c;
                FatIndices data;

                string delimiter = "/";
                string value;

                data.a.a = stoi(a.substr(0, a.find(delimiter)));
                a.erase(0, a.find(delimiter) + delimiter.length());
                data.a.b = stoi(a.substr(0, a.find(delimiter)));
                a.erase(0, a.find(delimiter) + delimiter.length());
                data.a.c = stoi(a.substr(0, a.find(delimiter)));
                a.erase(0, a.find(delimiter) + delimiter.length());

                data.b.a = stoi(b.substr(0, b.find(delimiter)));
                b.erase(0, b.find(delimiter) + delimiter.length());
                data.b.b = stoi(b.substr(0, b.find(delimiter)));
                b.erase(0, b.find(delimiter) + delimiter.length());
                data.b.c = stoi(b.substr(0, b.find(delimiter)));
                b.erase(0, b.find(delimiter) + delimiter.length());

                data.c.a = stoi(c.substr(0, c.find(delimiter)));
                c.erase(0, c.find(delimiter) + delimiter.length());
                data.c.b = stoi(c.substr(0, c.find(delimiter)));
                c.erase(0, c.find(delimiter) + delimiter.length());
                data.c.c = stoi(c.substr(0, c.find(delimiter)));
                c.erase(0, c.find(delimiter) + delimiter.length());
                
                edittedFileIndices.push_back(data);
            }
            else
            {
                int x = 0;
            }
        }
        edittedFile.close();
    }

    //vector<Vertex>  originalFileVertex;
    //vector<Texture> originalFileTexture;
    //vector<Normal>  originalFileNormal;
    //vector<Indices> originalFileIndices;
    //vector<Vertex>  edittedFileVertex;
    //vector<Texture> edittedFileTexture;
    //vector<Normal>  edittedFileNormal;
    //vector<FatIndices> edittedFileIndices;

    vector<APFVertex> baseAPF;
    baseAPF.resize(originalFileVertex.size());
    for (FatIndices node : edittedFileIndices)
    {
        baseAPF[node.a.a - 1].Pos =  edittedFileVertex[node.a.a - 1];
        baseAPF[node.a.a - 1].Tex = edittedFileTexture[node.a.b - 1];
        baseAPF[node.a.a - 1].Nor =  edittedFileNormal[node.a.c - 1];

        baseAPF[node.b.a - 1].Pos =  edittedFileVertex[node.b.a - 1];
        baseAPF[node.b.a - 1].Tex = edittedFileTexture[node.b.b - 1];
        baseAPF[node.b.a - 1].Nor =  edittedFileNormal[node.b.c - 1];

        baseAPF[node.c.a - 1].Pos =  edittedFileVertex[node.c.a - 1];
        baseAPF[node.c.a - 1].Tex = edittedFileTexture[node.c.b - 1];
        baseAPF[node.c.a - 1].Nor =  edittedFileNormal[node.c.c - 1];
    }

    std::ofstream resultFile;
    resultFile.open(resultPath.c_str(), ios::out | ios::binary);
    if (resultFile.is_open())
    {
        int APFVertexSize = sizeof(APFVertex);
        int VertexCount = baseAPF.size();
        int byteSize = APFVertexSize * VertexCount;
        resultFile.write((char*)&baseAPF[0], byteSize);
        resultFile.close();
    }

    vector<Indices> newFileIndices = originalFileIndices;
    for (Indices& index : newFileIndices)
    {
        --index.a;
        --index.b;
        --index.c;
    }

    std::ofstream indicesFile;
    indicesFile.open(indicesPath.c_str(), ios::out | ios::binary);
    if (indicesFile.is_open())
    {
        int IndicesSize = sizeof(Indices);
        int IndicesCount = newFileIndices.size();
        int byteSize = IndicesSize * IndicesCount;
        indicesFile.write((char*)&newFileIndices[0], byteSize);
        indicesFile.close();
    }
}
