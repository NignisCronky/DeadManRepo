#pragma pack_matrix(row_major)

struct V_IN
{
	float4 posIn : POSITION;
	float3 normIn : NORMAL;
	float2 uvIn : UV;
};
struct V_OUT
{
	float4 posOut : SV_POSITION;
	float3 normOut : NORMAL;
	float2 uvOut : UV;
};

cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
}
cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

V_OUT main(V_IN input)
{
	V_OUT output = (V_OUT)0;

	// ensures translation is preserved during matrix multiply  
	float4 localH = float4(input.posIn.xyz, 1);

	// move local space vertex from vertex buffer into world space.
	localH = mul(localH, worldMatrix);
	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectionMatrix);
	output.posOut = localH;

	output.uvOut = input.uvIn;
	output.normOut = mul(input.normIn, worldMatrix);

	return output; // send projected vertex to the rasterizer stage
}