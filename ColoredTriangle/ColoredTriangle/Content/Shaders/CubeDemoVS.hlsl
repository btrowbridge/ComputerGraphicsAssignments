cbuffer CBufferPerFrame 
{
	float4x4 WorldViewProjectionMatrix;
};


struct VS_INPUT
{
	float4 ObjectPosition : POSITION;
	float4 Color : COLOR;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};

VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT OUT = (VS_OUTPUT)0;
	
	OUT.Position = mul(IN.ObjectPosition, WorldViewProjectionMatrix);

	OUT.Color = IN.Color;

	return OUT;
}