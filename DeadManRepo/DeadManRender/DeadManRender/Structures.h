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