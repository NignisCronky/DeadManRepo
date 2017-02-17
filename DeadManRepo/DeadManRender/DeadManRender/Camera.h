#pragma once
#include <DirectXMath.h>

class Camera
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;

	DirectX::XMFLOAT3  avatarPosition = { 0, 0, -1 };
	DirectX::XMFLOAT3 cameraPosition = { 0.5f,1.0f,2.0f };

	float avatarYaw = 0;

	DirectX::XMFLOAT3 cameraReference = { 5.0f, 0.0f, 5.0f };

	float rotationSpeed = 1.0f / 380.0f;
	float forwardSpeed = 1.0f / 380.0f;

	float viewAngle = DirectX::XM_PI / 4.0f;

	float nearClip = 1.0f;
	float farClip = 2000.0f;
public:
	void InitCamera();
	void CameraUPdadte();
	void UpdateCamera();
	Camera();
	~Camera();
};

