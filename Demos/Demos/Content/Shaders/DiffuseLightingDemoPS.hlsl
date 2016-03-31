cbuffer CBufferPerFrame
{
	float4 LightDirection;
	float4 AmbientColor;

};

SamplerState ColorSampler;
Texture2D ColorTexture;

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinates : TEXCOORD;
	float3 Normal : NORMAL;
};

float4 main(VS_OUTPUT IN) : SV_TARGET
{
	float4 color =(ColorTexture.Sample(ColorSampler,IN.TextureCoordinates));

	float3 normal = normalize(IN.Normal);
	float3 lightDirection = normalize(LightDirection.rbg);

	float n_dot_l = dot(normal, lightDirection);
	
	float3 ambient = color.rgb * AmbientColor.rbg * AmbientColor.a;
	float3 diffuse = color.rgb * saturate(n_dot_l);

	float4 OUT = float4(saturate(ambient + diffuse), color.a);
	

	return OUT;
}