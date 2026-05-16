
RWStructuredBuffer<float> exampleBuffer : register(u0, space1);

[numthreads(8, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	exampleBuffer[DTid.x] = DTid.x;
}