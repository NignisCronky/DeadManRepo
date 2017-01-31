SamplerState sam;
TextureCube cubeTex;

struct PixelShaderInput
{
	float4 worldPos : WORLDPOS;
	float4 pos : SV_POSITION;
	float4 uv : UV;
	float4 color : COLOR;
	float4 norm : NORMAL;
};

float4 main(PixelShaderInput _input) : SV_TARGET
{
	return cubeTex.Sample(sam, _input.color);
}