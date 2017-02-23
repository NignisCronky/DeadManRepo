#pragma once

#include <Windows.h>
#include <vector>
#include <DirectXMath.h>
#include "Structures.h"
//using namespace DirectX;


#define PI		3.14159f



float Degrees_To_Radians(float _degrees)
{
	return _degrees * PI / 180.0f;
}

float Radians_To_Degrees(float _radians)
{
	return _radians * 180.0f / PI;
}

void GetProjectionMatrix_4X4(float _FOVangleinDegrees, float _zNear, float _zFar, float _aspectRatio, DirectX::XMMATRIX &_matrix)
{
	float HalfOfDegrees = _FOVangleinDegrees * 0.5f;
	float radians = Degrees_To_Radians(HalfOfDegrees);
	float Cos = (float)cos(radians),
		Sin = (float)sin(radians);
	float coTangentOfFOV = 1 / tanf(radians);
	//float coTangentOfFOV = (Cos / Sin);

	float Yscale = coTangentOfFOV;
	float Xscale = coTangentOfFOV * _aspectRatio;
	

	ZeroMemory(&_matrix, sizeof(DirectX::XMMATRIX));

	_matrix.r[0].m128_f32[0] = Xscale;
	_matrix.r[1].m128_f32[1] = Yscale;
	_matrix.r[2].m128_f32[2] = (_zFar / (_zFar - _zNear));
	_matrix.r[2].m128_f32[3] = 1;
	_matrix.r[3].m128_f32[2] = (-(_zFar * _zNear) / (_zFar - _zNear));
}