cbuffer constantBuffer : register(b0, space2)
{
    float2 MousePosition;
    float DeltaTime;
    float ElapsedTime;
    float2 Particles[2000];
};

RWTexture2D<float4> renderTexture : register(u0, space1);
RWStructuredBuffer<float> exampleBuffer : register(u1, space1);

static float metaballStr(float2 p, float2 centre, float r)
{
    return r / length(p - centre);
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 groupID : SV_GroupThreadID)
{
	exampleBuffer[DTid.x] = DTid.x;
    
    float w = 0.0f;
    float h = 0.0f;
    renderTexture.GetDimensions(w, h);
    
    float2 centre = float2((w) * 0.5f, (h) * 0.5f);
    
    if (DTid.x >= w || DTid.y >= h)
    {
        return;
    }
    
    float nx = DTid.x / w;
    float ny = DTid.y / h;
    
    float r = 1.0f;
    float sdf = 0.0f;
        
    renderTexture[DTid.xy] = saturate(float4(0.0f, 0.0f, 0.0f, 1.0f));
    
    [unroll(500)]
    for (int i = 0; i < 500; i++)
    {
        if (Particles[i].x < 1.0f || Particles[i].y < 1.0f)
        {
            continue;
        }
                
        if (distance(float2(DTid.x, DTid.y), Particles[i]) < 2.0f)
        {
            renderTexture[DTid.xy] = saturate(float4(0.0f, 1.0f, 0.0f, 1.0f));
        }
        
        sdf += metaballStr(float2(DTid.x, DTid.y), Particles[i], r);
    }
    
    sdf += metaballStr(float2(DTid.x, DTid.y), MousePosition, r);
    
    if (sdf > 0.5f)
    {
        renderTexture[DTid.xy] = saturate(float4(1.0f, 0.0f, 0.0f, 1.0f));
    }
    
    //[unroll(4000)]
    //for (int i = 0; i < 4000; i++)
    //{
    //    sdf += metaballStr(float2(DTid.x, DTid.y), Particles[i], r); 
    //}
    //
    //sdf += metaballStr(float2(DTid.x, DTid.y), MousePosition, r);
    //
    //if (sdf >= -1.0f && sdf <= 1.0f)
    //{
    //    renderTexture[DTid.xy] = saturate(float4(sdf, sdf, sdf, 1.0f));
    //}
}