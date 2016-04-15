
cbuffer CBufferPerFrame
{
	float4 AmbientColor;
	float3 LightColor;	
};

cbuffer CbufferPerObject
{
	float3 SpecularColor;
	float SpecularPower;
};


SamplerState ColorSampler;
Texture2D ColorTexture;
Texture2D SpecularMap;

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinates : TEXCOORD;
	float3 Normal : NORMAL;
	float3 ViewDirection : VIEWDIR;
	float3 LightDirection : LIGHTDIR;
	float Attenuation : ATTENUATION;
};

float4 main(VS_OUTPUT IN) : SV_TARGET
{
	float n_dot_l = dot(In.Normal, IN.LightDirection);
	float3 halfVector = normalize(IN.LightDirection + IN.ViewDirection);
	float n_dot_h = dot(IN.Normal, halfVector);

	float4 lightCoefficients = lit(n_dot_l, n_dot_h, SpecularPower);
	float specularClamp = SpecularMap.Sample(TextureSampler, In.TextureCoordinates).x;
	float4 color = (ColorTexture.Sample(ColorSampler, IN.TextureCoordinates));

	float3 ambient = color.rgb * AmbientColor.rbg * AmbientColor.a;
	float3 diffuse = color.rgb * saturate(n_dot_l) * LightColor * IN.Attenuation;
	float3 specular = min(lightCoefficient.z, specularClamp) * SpecularColor * IN.Attenuation;
	//float3 specular = min(lightCoefficient.z, color.a) * SpecularColor;

	return float4(saturate(ambient + diffuse + specular), color.a);

}