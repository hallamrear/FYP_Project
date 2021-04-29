SamplerState linearSampler : register(s0);
//TODO : Double check texture sampler exists

struct VS_OUTPUT
{
    float4 PositionWVP : SV_POSITION;
    //float4 PositionW : POSITION;
    float2 TexCoords : TEXCOORD;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 WorldMatrix;
    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
};

