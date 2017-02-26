#include <Windows.h>
#include "Skybox.h"

#include "Skybox_VS.csh"
#include "Skybox_PS.csh"
#include "DDSTextureLoader.h"

Skybox::Skybox()
{
}


Skybox::~Skybox()
{
}

void Skybox::Initialize(ID3D11Device *_device, ID3D11DeviceContext *_context, SEND_TO_SCENE *_toSceneShader)
{
	device = _device;
	toSceneShader = _toSceneShader;
	context = _context;


	const unsigned int SKYBOXVERTS = 8;
	float SKYBOXSIZE = 1;
	const unsigned int SKYBOXINDECIES = 36;

	SKYBOX_VERTEX SkyBoxVerts[SKYBOXVERTS] = {
		{ DirectX::XMFLOAT4(-SKYBOXSIZE, -SKYBOXSIZE, -SKYBOXSIZE, 0.0f), (0.0f, 0.0f, 0.0f, 0.0f), (0.0f, 0.0f, 0.0f) },
		{ DirectX::XMFLOAT4(-SKYBOXSIZE, -SKYBOXSIZE, SKYBOXSIZE, 0.0f), (0.0f, 0.0f, 1.0f, 0.0f), (0.0f, 0.0f, 0.0f) },
		{ DirectX::XMFLOAT4(-SKYBOXSIZE, SKYBOXSIZE, -SKYBOXSIZE, 0.0f), (0.0f, 1.0f, 0.0f, 0.0f), (0.0f, 0.0f, 0.0f) },
		{ DirectX::XMFLOAT4(-SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE, 0.0f), (0.0f, 1.0f, 1.0f, 0.0f), (0.0f, 0.0f, 0.0f) },
		{ DirectX::XMFLOAT4(SKYBOXSIZE, -SKYBOXSIZE, -SKYBOXSIZE, 0.0f), (1.0f, 0.0f, 0.0f, 0.0f), (0.0f, 0.0f, 0.0f) },
		{ DirectX::XMFLOAT4(SKYBOXSIZE, -SKYBOXSIZE, SKYBOXSIZE, 0.0f), (1.0f, 0.0f, 1.0f, 0.0f), (0.0f, 0.0f, 0.0f) },
		{ DirectX::XMFLOAT4(SKYBOXSIZE, SKYBOXSIZE, -SKYBOXSIZE, 0.0f), (1.0f, 1.0f, 0.0f, 0.0f), (0.0f, 0.0f, 0.0f) },
		{ DirectX::XMFLOAT4(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE, 0.0f), (1.0f, 1.0f, 1.0f, 0.0f), (0.0f, 0.0f, 0.0f) },
	};

	unsigned int SkyBoxIndices[] = {
		0, 2, 1, 1, 2, 3,	//	-x
		4, 5, 6, 5, 7, 6,	//	+x
		0, 1, 5, 0, 5, 4,	//	-y
		2, 6, 7, 2, 7, 3,	//	+y
		0, 4, 6, 0, 6, 2,	//	-z
		1, 3, 7, 1, 7, 5,	//	+z
	};

	D3D11_BUFFER_DESC Skyboxbufferdesc;
	ZeroMemory(&Skyboxbufferdesc, sizeof(D3D11_BUFFER_DESC));

	Skyboxbufferdesc.Usage = D3D11_USAGE_IMMUTABLE;
	Skyboxbufferdesc.ByteWidth = sizeof(SKYBOX_VERTEX) * SKYBOXINDECIES;
	Skyboxbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Skyboxbufferdesc.CPUAccessFlags = NULL;
	Skyboxbufferdesc.MiscFlags = NULL;

	Skyboxbufferdesc.StructureByteStride = NULL;

	D3D11_SUBRESOURCE_DATA Skyboxsubresourcedata = {};
	Skyboxsubresourcedata.pSysMem = SkyBoxVerts;
	//	Skyboxsubresourcedata.pSysMem = SkyBox;

	// Index Buffer
	D3D11_BUFFER_DESC indexBufferDesc;

	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 12 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexSkyboxSubresourcedata = {};
	indexSkyboxSubresourcedata.pSysMem = SkyBoxIndices;
	hr = device->CreateBuffer(&indexBufferDesc, &indexSkyboxSubresourcedata, &skyboxindexbuffer);
	assert(hr == S_OK);

	// Vertex Buffer
	hr = device->CreateBuffer(&Skyboxbufferdesc, &Skyboxsubresourcedata, &skyboxvertexbuffer);
	assert(hr == S_OK);

	D3D11_BUFFER_DESC skyboxconstobjdesc;
	ZeroMemory(&skyboxconstobjdesc, sizeof(D3D11_BUFFER_DESC));

	skyboxconstobjdesc.Usage = D3D11_USAGE_DYNAMIC;
	skyboxconstobjdesc.ByteWidth = sizeof(SEND_TO_OBJECT);
	skyboxconstobjdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	skyboxconstobjdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	skyboxconstobjdesc.MiscFlags = NULL;

	skyboxconstobjdesc.StructureByteStride = NULL;

	hr = device->CreateBuffer(&skyboxconstobjdesc, 0, &skyboxconstantbufferobject);
	assert(hr == S_OK);
	CreateSkyboxShaders();
}

void Skybox::CreateSkyboxShaders()
{
	InitializeSkyboxTexture();
	hr = device->CreateVertexShader(Skybox_VS, sizeof(Skybox_VS), NULL, &skyboxvertex_shader);
	assert(hr == S_OK);

	hr = device->CreatePixelShader(Skybox_PS, sizeof(Skybox_PS), NULL, &skyboxpixel_shader);
	assert(hr == S_OK);

	D3D11_SAMPLER_DESC samplerdesc;
	DefaultSamplerDesc(samplerdesc);

	hr = device->CreateSamplerState(&samplerdesc, &skyboxSamplerState);
	assert(hr == S_OK);

	D3D11_INPUT_ELEMENT_DESC inputlayoutdesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UVW", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = device->CreateInputLayout(inputlayoutdesc, ARRAYSIZE(inputlayoutdesc), Skybox_VS, sizeof(Skybox_VS), &skyboxinputlayout);
	assert(hr == S_OK);

	ZeroMemory(&toskyboxObjectShader, sizeof(SEND_TO_OBJECT));

	toskyboxObjectShader.worldMatrix.r[0].m128_f32[0] = 50;
	toskyboxObjectShader.worldMatrix.r[1].m128_f32[1] = 50;
	toskyboxObjectShader.worldMatrix.r[2].m128_f32[2] = 50;
	toskyboxObjectShader.worldMatrix.r[3].m128_f32[3] = 1;

	toskyboxObjectShader.worldMatrix = DirectX::XMMatrixMultiply(toskyboxObjectShader.worldMatrix, DirectX::XMMatrixIdentity());

}

void Skybox::Draw()
{
	// Move skybox
	toSceneShader->viewMatrix = DirectX::XMMatrixInverse(0, toSceneShader->viewMatrix);
	toskyboxObjectShader.worldMatrix.r[3] = toSceneShader->viewMatrix.r[3];
	//	toskyboxObjectShader.worldMatrix.r[2].m128_f32[0] = toSceneShader.viewMatrix.r[2].m128_f32[0];
	//	toskyboxObjectShader.worldMatrix.r[2].m128_f32[1] = toSceneShader.viewMatrix.r[2].m128_f32[1];
	//	toskyboxObjectShader.worldMatrix.r[2].m128_f32[2] = toSceneShader.viewMatrix.r[2].m128_f32[2];

	toSceneShader->viewMatrix = XMMatrixInverse(0, toSceneShader->viewMatrix);

	context->IASetInputLayout(skyboxinputlayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Triangle constant buffer object
	context->VSSetConstantBuffers(0, 1, &skyboxconstantbufferobject);

	//attempt to apply the texture
	context->VSSetShader(skyboxvertex_shader, NULL, NULL);
	context->PSSetShader(skyboxpixel_shader, NULL, NULL);
	context->PSSetShaderResources(0, 1, &skyboxShaderResourceView_textureview);

	//	ID3D11SamplerState;
	context->PSSetSamplers(0, 1, &skyboxSamplerState);

	context->IASetIndexBuffer(skyboxindexbuffer, DXGI_FORMAT_R32_UINT, NULL);

	D3D11_MAPPED_SUBRESOURCE mapObject;
	hr = context->Map(skyboxconstantbufferobject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObject);
	assert(hr == S_OK);

	memcpy(mapObject.pData, &toskyboxObjectShader, sizeof(SEND_TO_OBJECT));
	context->Unmap(skyboxconstantbufferobject, 0);

	const unsigned int sizeofvertexstuff = sizeof(SKYBOX_VERTEX);
	const unsigned int nextpointinmemory = 0;
	context->IASetVertexBuffers(0, 1, &skyboxvertexbuffer, &sizeofvertexstuff, &nextpointinmemory);

	context->DrawIndexed(36, 0, 0);
//	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

}

void Skybox::InitializeSkyboxTexture()
{
	hr = CreateDDSTextureFromFile(device, L"SunsetSkybox.dds", NULL, &skyboxShaderResourceView_textureview);
	assert(hr == S_OK);
}

void Skybox::DefaultSamplerDesc(D3D11_SAMPLER_DESC &_desc)
{
	ZeroMemory(&_desc, sizeof(D3D11_SAMPLER_DESC));

	_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	_desc.MinLOD = (-FLT_MAX);
	_desc.MaxLOD = FLT_MAX;
	_desc.MipLODBias = 0.0f;
	_desc.MaxAnisotropy = 1;
	_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	_desc.BorderColor[0] = 1;
	_desc.BorderColor[1] = 1;
	_desc.BorderColor[2] = 1;
	_desc.BorderColor[3] = 1;
}