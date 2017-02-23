#pragma once

#include <d3D11.h>
#include "Structures.h"

class Platform
{
private:
	ID3D11Buffer					*skyboxvertexbuffer = nullptr;
	ID3D11InputLayout				*skyboxinputlayout = nullptr;

	ID3D11Buffer					*skyboxconstantbufferobject = nullptr;

	SEND_TO_OBJECT					toskyboxObjectShader;

	// Basic Shaders
	ID3D11VertexShader				*skyboxvertex_shader = nullptr;
	ID3D11PixelShader				*skyboxpixel_shader = nullptr;

	// Skybox texture
	ID3D11Resource					*skyboxResource_texture = nullptr;	// Redundent
	ID3D11ShaderResourceView		*skyboxShaderResourceView_textureview = nullptr;
	ID3D11SamplerState				*skyboxSamplerState = nullptr;

	ID3D11Buffer					*skyboxindexbuffer = nullptr;
public:
	Platform();
	~Platform();
	void Initialize();
	void Draw();
};

