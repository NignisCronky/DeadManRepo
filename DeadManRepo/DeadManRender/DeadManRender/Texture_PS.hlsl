struct Light
{
	float3 direction;
	float pad;
	float4 ambient;
	float4 diffues;
};
cbuffer LIGHTS : register(b0)
{
	Light light;
}

struct INPUT_VERTEX
{
	float4 Pos : SV_POSITION;
	float3 norm : NORMAL;
	float2 uv : UV;
};

Texture2D baseTexture;
SamplerState filter;

float4 main(INPUT_VERTEX input) : SV_TARGET
{
	input.norm = normalize(input.norm);

	float3 colorToReturn;

	return baseTexture.Sample(filter, input.uv.xy);
}