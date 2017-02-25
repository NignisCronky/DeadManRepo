struct INPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float4 colorOut : COLOR;
	float3 uvw : UVW;
};

TextureCube cubeTexture;
Texture2D baseTexture;
SamplerState filter;

float4 main(INPUT_VERTEX input) : SV_TARGET
{
	return cubeTexture.Sample(filter, input.uvw.xyz);
}