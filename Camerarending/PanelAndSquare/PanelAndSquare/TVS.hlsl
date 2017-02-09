


struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	row_major matrix model;
	row_major matrix view;
	row_major matrix projection;
};



VOut main(float4 position : POSITION, float4 color : COLOR)
{
	VOut output;
	float4 pos = float4(position);
	pos.w = 1;
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	
	output.position = pos;
	output.color = color;

	return output;
}