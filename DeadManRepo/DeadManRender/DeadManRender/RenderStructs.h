#pragma once
/////////////////////////////////////////////////////////////
//   This is what i use to bring everything to projection space
/////////////////////////////////////////////////////////////
// A constant buffer that stores the three basic column-major matrices for composing geometry.
//cbuffer ModelViewProjectionConstantBuffer : register(b0)
//{
//	matrix model;
//	matrix view;
//	matrix projection;
//};

/////////////////////////////////////////////////////////////
//   This is vertex without bones.
/////////////////////////////////////////////////////////////
//// Per-vertex data used as input to the vertex shader.
//struct VertexShaderInput
//{
//	float4 pos : POSITION;
//	float4 uv : UV;
//	float4 color : COLOR;
//	float4 norm : NORMAL;
//};

/////////////////////////////////////////////////////////////
//   light class for pixel shader. the position.w holds what kind of light it is
//   1= ambient;   2= directional;   3 = point;   4 = spotlight;
//   0 = is no light...
/////////////////////////////////////////////////////////////
//struct genLight
//{
//	DirectX::XMFLOAT4 position;
//	DirectX::XMFLOAT4 direction;
//	DirectX::XMFLOAT4 color;
//	DirectX::XMFLOAT4 specularColor;
//	DirectX::XMFLOAT4 camera;
//	DirectX::XMFLOAT4 coneRatio;
//};