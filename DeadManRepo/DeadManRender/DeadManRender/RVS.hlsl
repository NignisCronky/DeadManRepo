


struct VOut
{
	float4 position : SV_POSITION;
	float2 uvs : UVS;
	float4 color: COLOR;
};

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	row_major matrix model;
	row_major matrix view;
	row_major matrix projection;
};



VOut main(float4 position : POSITION, 
		unsigned int AmmountOfIndices : INDICIES, 
		float4 weights : BLENDWEIGHTS,
		unsigned int4 Indices: BONEINDICES,
		float3 normals: NORMALS,
		float2 uvs: UVS
)
{
	VOut output;
	float4 pos = float4(position);
	pos.w = 1;
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	
	output.uvs = uvs;
	output.position = pos;
	output.color = (1, 0, 0, 1);
	return output;
}