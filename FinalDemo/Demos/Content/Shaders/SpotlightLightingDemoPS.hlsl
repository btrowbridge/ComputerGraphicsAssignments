
cbuffer CBufferPerFrame
{
	float4 AmbientColor;
	float3 LightColor;	
	float3 LightLookAt;
	float SpotLightInnerAngle;
	float SpotLightOuterAngle;
};

cbuffer CbufferPerObject
{
	float3 SpecularColor;
	float SpecularPower;
};


SamplerState ColorSampler;
Texture2D ColorTexture;


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
	float n_dot_l = dot(IN.Normal, IN.LightDirection);
	float3 halfVector = normalize(IN.LightDirection + IN.ViewDirection);
	float n_dot_h = dot(IN.Normal, halfVector);

	float4 lightCoefficients = lit(n_dot_l, n_dot_h, SpecularPower);
	float specularClamp = ColorTexture.Sample(ColorSampler, IN.TextureCoordinates).w;
	float3 color = (ColorTexture.Sample(ColorSampler, IN.TextureCoordinates).xyz);

	float3 ambient = color.rgb * AmbientColor.rbg * AmbientColor.a;
	float3 diffuse = color.rgb * saturate(n_dot_l) * LightColor;
	float3 specular = min(lightCoefficients.z, specularClamp) * SpecularColor;
	

	float lightAngle = dot(LightLookAt, IN.LightDirection);
	float lightAngleCoefficient = smoothstep(SpotLightOuterAngle, SpotLightInnerAngle, lightAngle);
    float spotFactor = saturate(lightAngleCoefficient);
    //float spotFactor = (lightAngle > 0.0f ? lightAngleCoefficient : 0.0);

	return float4(saturate(ambient + spotFactor * IN.Attenuation * (diffuse + specular)), 1.0f);

}