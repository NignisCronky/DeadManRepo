#pragma once

#include <d3D11.h>
#include "Structures.h"

struct TEXTURE_VERTEX
{
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT3 normal;
	float uv[2];
};

class Platform
{
private:
	HRESULT							hr;
	ID3D11Device					*device = nullptr;
//	DirectX::XMMATRIX				worldTranslate;

	ID3D11Buffer					*Platformindexbuffer = nullptr;
	ID3D11Buffer					*platformvertexbuffer = nullptr;
	ID3D11Buffer					*platformconstantbufferobject = nullptr;
	ID3D11InputLayout				*platforminputlayout = nullptr;

	ID3D11VertexShader				*platformvertex_shader = nullptr;
	ID3D11PixelShader				*platformpixel_shader = nullptr;
	ID3D11SamplerState				*PlatformSamplerState = nullptr;
	ID3D11ShaderResourceView		*PlatformShaderResourceView_textureview = nullptr;
	DirectX::XMMATRIX				worldTranslate;

	SEND_TO_OBJECT					toplatformObjectShader;

	void CreatePlatformShaders();
	void DefaultSamplerDesc(D3D11_SAMPLER_DESC &_desc);
	void InitializePlatformTexture();
public:
//	Platform(ID3D11Device *_device);
	void Initialize(ID3D11Device *_device);
	void Draw(ID3D11DeviceContext *_context);
	Platform();
	~Platform();
};

