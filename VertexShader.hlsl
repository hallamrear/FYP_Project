#include "ShaderStructures.hlsli"

struct VS_INPUT
{
    float3 Position : POSITION0;
    float2 TexCoords : TEXCOORD0;
    float3 InstancePos : TEXCOORD1;
};

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
        
    float4 pos = float4(input.Position.xyz, 1.0f);    
    
    pos.x += input.InstancePos.x;
    pos.y += input.InstancePos.y;
    
    //output.PositionW = mul(pos, WorldMatrix);
    
    pos = mul(pos, WorldMatrix);
    pos = mul(pos, ViewMatrix);
    pos = mul(pos, ProjectionMatrix);
    output.PositionWVP = pos;
    
    output.TexCoords = input.TexCoords;
       
    return output;
}