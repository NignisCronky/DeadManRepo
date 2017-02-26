#pragma once
#include <DirectXMath.h>

struct SEND_TO_SCENE
{
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
};
struct SEND_TO_OBJECT
{
	DirectX::XMMATRIX worldMatrix;
};
struct TEXTURE_VERTEX
{
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT3 normal;
	float uv[2];
};
struct SKYBOX_VERTEX
{
	DirectX::XMFLOAT4 pos;
	float color[4];
	float uvw[3];
};