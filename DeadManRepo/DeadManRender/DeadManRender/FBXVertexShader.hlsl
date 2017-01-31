// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float4 pos : POSITION;
	float4 uv : UV;
	float4 color : COLOR;
	float4 norm : NORMAL;
	//row_major float4x4 inst : INSTANCE;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	//pos used for lighting
	float4 worldPos : WORLDPOS;
	//pos for camera
	float4 pos : SV_POSITION;
	float4 uv : UV;
	float4 color : COLOR;
	float4 norm : NORMAL;
};

PixelShaderInput main(VertexShaderInput input)
{
	//need to implement bones

	PixelShaderInput output;
	float4 pos = float4(input.pos);

	//pos = mul(pos, input.inst);
	pos = mul(pos, model);
	output.worldPos = pos;
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	output.uv = input.uv;
	output.color = input.pos;

	float4 norm = float4(normalize(input.norm.xyz), 0);
	//norm = mul(norm, input.inst);
	norm = mul(norm, model);
	norm = float4(normalize(norm.xyz), 0);
	output.norm = norm;

	return output;
}