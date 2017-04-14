//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer cbViewMatrix : register(b0)
{
    matrix View;
};

cbuffer cbProjectionMatrix : register(b1)
{
    matrix Projection;
};

cbuffer cbWorldMatrix : register(b2)
{
    matrix World;
};

cbuffer cbColorCorrectionInfo : register(b3)
{
    float4 ColorMax;
};


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
    float4 Nor : NORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float4 Col : COLOR;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul(output.Pos, Projection);
    output.Tex = input.Tex;
    output.Col = input.Nor;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
    // return input.Col;                             // normal output
    float4 color = txDiffuse.Sample(samLinear, input.Tex);
    color.x *= ColorMax.x;
    color.y *= ColorMax.y;
    color.z *= ColorMax.z;
    color.w *= ColorMax.w;
    return color;  // texture output
    //return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
