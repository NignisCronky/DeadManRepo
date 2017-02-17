#include "Camera.h"
#include <Windows.h>
Camera::Camera()
{
}
Camera::~Camera()
{
}
void Camera::UpdateCamera()
{
	DirectX::XMFLOAT3 debug;
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(avatarYaw);
	DirectX::XMVECTOR transformedReference = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&avatarPosition), rotationMatrix);
	DirectX::XMStoreFloat3(&debug, transformedReference);
	float Debug1[3] = { debug.x, debug.y, debug.z };
	DirectX::XMVECTOR cameraLookat = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&cameraPosition), transformedReference);
	DirectX::XMStoreFloat3(&debug, cameraLookat);
	float Debug2[3] = { debug.x, debug.y, debug.z };
	DirectX::XMVECTOR UP = { 0.0f, 1.0f, 0.0f };
	DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&cameraPosition), cameraLookat, UP));
}
void Camera::InitCamera()
{
	DirectX::XMFLOAT3 debug;
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(avatarYaw);
	DirectX::XMVECTOR transformedReference = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&cameraReference), rotationMatrix);
	DirectX::XMStoreFloat3(&debug, transformedReference);
	float Debug1[3] = { debug.x, debug.y, debug.z };
	DirectX::XMVECTOR cameraLookat = { 0.5f,-0.6f, -0.5f };
	DirectX::XMStoreFloat3(&debug, cameraLookat);
	float Debug2[3] = { debug.x, debug.y, debug.z };
	DirectX::XMVECTOR UP = { 0.0f, 1.0f, 0.0f };
	DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&cameraPosition), cameraLookat, UP));
	DirectX::XMStoreFloat4x4(&proj, DirectX::XMMatrixPerspectiveFovLH(viewAngle, (800.0f / 1100.0f), nearClip, farClip));
}
void Camera::CameraUPdadte()
{
	if (GetAsyncKeyState(0x57)) {//W - Move Forward
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, -forwardSpeed);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(temp_camera, translation);
		XMStoreFloat4x4(&view, result);
	}
	if (GetAsyncKeyState(0x53)) {//S - Move Backward
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, +forwardSpeed);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(temp_camera, translation);
		XMStoreFloat4x4(&view, result);
	}
	if (GetAsyncKeyState(0x41)) {//A - Strafe Left
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(forwardSpeed, 0.0f, 0.0f);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(temp_camera, translation);
		XMStoreFloat4x4(&view, result);
	}
	if (GetAsyncKeyState(0x44)) {//D - Strafe Right
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(-forwardSpeed, 0.0f, 0.0f);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(temp_camera, translation);
		XMStoreFloat4x4(&view, result);
	}
	if (GetAsyncKeyState(VK_SPACE)) {//Space - Move Up
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, -forwardSpeed, 0.0f);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(temp_camera, translation);
		XMStoreFloat4x4(&view, result);
	}
	if (GetAsyncKeyState(VK_LSHIFT)) {//Shift - Move Down
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, forwardSpeed, 0.0f);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(temp_camera, translation);
		XMStoreFloat4x4(&view, result);
	}
	if (GetAsyncKeyState(VK_RIGHT)) {//Tab - rotate right
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(-rotationSpeed);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(temp_camera, rotationMatrix);
		XMStoreFloat4x4(&view, result);
	}
	if (GetAsyncKeyState(VK_LEFT)) {//Tab - rotate left
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(rotationSpeed);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(temp_camera, rotationMatrix);
		XMStoreFloat4x4(&view, result);
	}
	if (GetAsyncKeyState(VK_UP)) {//up arrow - rotate up
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationX(rotationSpeed);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(temp_camera, rotationMatrix);
		XMStoreFloat4x4(&view, result);
	}
	if (GetAsyncKeyState(VK_DOWN)) {//Tab - rotate down
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationX(-rotationSpeed);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(temp_camera, rotationMatrix);
		XMStoreFloat4x4(&view, result);
	}
}