// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

cbuffer BoneCB : register(b1)
{
	matrix BoneOffset[40];
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float4 pos : POSITION;
	float4 uv : UV;
	float4 normal : NORMAL;
	float4 blendWeights : BLENDWEIGHT;
	uint4 boneIndices : BLENDINDICES;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 uv : UV;
	float4 normal : NORMAL;
	float4 worldPos : WORLDPOS;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = mul(BoneOffset[input.boneIndices.x], input.pos) * input.blendWeights.x;
	pos += mul(BoneOffset[input.boneIndices.y], input.pos) * input.blendWeights.y;
	pos += mul(BoneOffset[input.boneIndices.z], input.pos) * input.blendWeights.z;
	pos += mul(BoneOffset[input.boneIndices.w], input.pos) * input.blendWeights.w;

	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	output.worldPos = pos;

	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	// Pass the color through without modification.
	output.uv = input.uv;
	output.normal = mul(input.normal, model);
	return output;
}
