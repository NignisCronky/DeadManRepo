#pragma once

#include <d3D11.h>
#include "Structures.h"

class Skybox
{
private:
	HRESULT							hr;
	ID3D11Device					*device = nullptr;
	ID3D11DeviceContext				*context = nullptr;
	SEND_TO_SCENE					*toSceneShader;

	SEND_TO_OBJECT					toskyboxObjectShader;

	ID3D11Buffer					*skyboxindexbuffer = nullptr;
	ID3D11Buffer					*skyboxvertexbuffer = nullptr;
	ID3D11Buffer					*skyboxconstantbufferobject = nullptr;

	ID3D11VertexShader				*skyboxvertex_shader = nullptr;
	ID3D11PixelShader				*skyboxpixel_shader = nullptr;
	ID3D11SamplerState				*skyboxSamplerState = nullptr;
	ID3D11InputLayout				*skyboxinputlayout = nullptr;
	ID3D11ShaderResourceView		*skyboxShaderResourceView_textureview = nullptr;

	void CreateSkyboxShaders();
	void InitializeSkyboxTexture();
	void DefaultSamplerDesc(D3D11_SAMPLER_DESC &_desc);
public:
	void Initialize(ID3D11Device *_device, ID3D11DeviceContext *_context, SEND_TO_SCENE *_toSceneShader);
	void Draw();
	Skybox();
	~Skybox();
};

