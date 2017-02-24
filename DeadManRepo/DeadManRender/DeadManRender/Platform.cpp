#include <Windows.h>
#include "Platform.h"
#include "DDSTextureLoader.h"

//#pragma comment (lib, "d3D11.lib")
//#pragma comment (lib, "dxgi.lib")
//#pragma comment (lib, "d3dcompiler.lib")

#include "Texture_VS.csh"
#include "Texture_PS.csh"


Platform::Platform()
{
	worldTranslate = DirectX::XMMatrixIdentity();
}

void Platform::Initialize(ID3D11Device *_device)
{
	device = _device;

	const float FLOORSIZE = 3.0f;
	const float DEPTHONGROUND = -2.0f;
	const float UVSIZE = 10.0f;

	//	TEXTURE_VERTEX Platform[2][NUM_TRIANEVERTS];
	TEXTURE_VERTEX PlatformVerts[4];
	ZeroMemory(&PlatformVerts, sizeof(TEXTURE_VERTEX) * 4);

	// Position 1
	PlatformVerts[0].pos.x = -FLOORSIZE;		//	O  1
	PlatformVerts[0].pos.y = DEPTHONGROUND;		//	1  1
	PlatformVerts[0].pos.z = -FLOORSIZE;
	PlatformVerts[0].pos.w = 0.0f;

	PlatformVerts[0].uv[0] = 0.0f;
	PlatformVerts[0].uv[1] = UVSIZE;

	// Position 2
	PlatformVerts[1].pos.x = -FLOORSIZE;		//	1  1
	PlatformVerts[1].pos.y = DEPTHONGROUND;		//	0  1
	PlatformVerts[1].pos.z = FLOORSIZE;
	PlatformVerts[1].pos.w = 0.0f;

	PlatformVerts[1].uv[0] = 0.0f;
	PlatformVerts[1].uv[1] = 0.0f;

	// Position 3
	PlatformVerts[2].pos.x = FLOORSIZE;
	PlatformVerts[2].pos.y = DEPTHONGROUND;
	PlatformVerts[2].pos.z = FLOORSIZE;
	PlatformVerts[2].pos.w = 0.0f;

	PlatformVerts[2].uv[0] = UVSIZE;
	PlatformVerts[2].uv[1] = 0.0f;

	// Position 4
	PlatformVerts[3].pos.x = FLOORSIZE;
	PlatformVerts[3].pos.y = DEPTHONGROUND;
	PlatformVerts[3].pos.z = -FLOORSIZE;
	PlatformVerts[3].pos.w = 0.0f;

	PlatformVerts[3].uv[0] = UVSIZE;
	PlatformVerts[3].uv[1] = UVSIZE;

	unsigned int PlatformIndices[6] = {
		0, 1, 2,
		0, 2, 3
	};

	D3D11_BUFFER_DESC bufferdesc;

	ZeroMemory(&bufferdesc, sizeof(D3D11_BUFFER_DESC));
	bufferdesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferdesc.CPUAccessFlags = NULL;
	bufferdesc.ByteWidth = sizeof(TEXTURE_VERTEX) * 6;
	bufferdesc.MiscFlags = NULL;
	bufferdesc.StructureByteStride = NULL;

	D3D11_SUBRESOURCE_DATA subresourcedata;
	ZeroMemory(&subresourcedata, sizeof(D3D11_SUBRESOURCE_DATA));

	// Index Buffer
	D3D11_BUFFER_DESC indexBufferDesc;

	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * 6;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexPlatformSubresourcedata = {};
	indexPlatformSubresourcedata.pSysMem = PlatformIndices;
	hr = device->CreateBuffer(&indexBufferDesc, &indexPlatformSubresourcedata, &Platformindexbuffer);
	assert(hr == S_OK);

	subresourcedata.pSysMem = PlatformVerts;

	hr = device->CreateBuffer(&bufferdesc, &subresourcedata, &platformvertexbuffer);
	assert(hr == S_OK);

	// Constant buffer stuff
	D3D11_BUFFER_DESC constobjdesc;

	ZeroMemory(&constobjdesc, sizeof(D3D11_BUFFER_DESC));
	constobjdesc.ByteWidth = sizeof(SEND_TO_OBJECT);
	constobjdesc.Usage = D3D11_USAGE_DYNAMIC;
	constobjdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constobjdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	constobjdesc.MiscFlags = NULL;
	constobjdesc.StructureByteStride = NULL;

	hr = device->CreateBuffer(&constobjdesc, NULL, &platformconstantbufferobject);
	assert(hr == S_OK);

	CreatePlatformShaders();
}
void Platform::CreatePlatformShaders()
{
	InitializePlatformTexture();

	hr = device->CreateVertexShader(Texture_VS, sizeof(Texture_VS), NULL, &platformvertex_shader);
	assert(hr == S_OK);
	hr = device->CreatePixelShader(Texture_PS, sizeof(Texture_PS), NULL, &platformpixel_shader);
	assert(hr == S_OK);

	D3D11_SAMPLER_DESC samplerdesc;
	DefaultSamplerDesc(samplerdesc);

	samplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = device->CreateSamplerState(&samplerdesc, &PlatformSamplerState);
	assert(hr == S_OK);

	D3D11_INPUT_ELEMENT_DESC inputlayoutdesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = device->CreateInputLayout(inputlayoutdesc, ARRAYSIZE(inputlayoutdesc), Texture_VS, sizeof(Texture_VS), &platforminputlayout);
	assert(hr == S_OK);

	//Object Matrix Stuff.
	ZeroMemory(&toplatformObjectShader, sizeof(SEND_TO_OBJECT));

	toplatformObjectShader.worldMatrix.r[0].m128_f32[0] = 20;
	toplatformObjectShader.worldMatrix.r[1].m128_f32[1] = 1;
	toplatformObjectShader.worldMatrix.r[2].m128_f32[2] = 20;
	toplatformObjectShader.worldMatrix.r[3].m128_f32[3] = 1;

	toplatformObjectShader.worldMatrix = XMMatrixMultiply(toplatformObjectShader.worldMatrix, worldTranslate);
}
void Platform::Draw(ID3D11DeviceContext	*context)
{
	context->OMSetBlendState(0, 0, 0xfffffff);
	context->IASetInputLayout(platforminputlayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Triangle constant buffer object
	context->VSSetConstantBuffers(0, 1, &platformconstantbufferobject);

	// apply the texture
	context->VSSetShader(platformvertex_shader, NULL, NULL);
	context->PSSetShader(platformpixel_shader, NULL, NULL);
	context->PSSetShaderResources(0, 1, &PlatformShaderResourceView_textureview);

	//	ID3D11SamplerState;
	context->PSSetSamplers(0, 1, &PlatformSamplerState);

	context->IASetIndexBuffer(Platformindexbuffer, DXGI_FORMAT_R32_UINT, NULL);

	D3D11_MAPPED_SUBRESOURCE mapObject;
	hr = context->Map(platformconstantbufferobject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObject);
	assert(hr == S_OK);

	memcpy(mapObject.pData, &toplatformObjectShader, sizeof(SEND_TO_OBJECT));
	context->Unmap(platformconstantbufferobject, 0);

	const unsigned int sizeofvertexstuff = sizeof(TEXTURE_VERTEX);
	const unsigned int nextpointinmemory = 0;
	context->IASetVertexBuffers(0, 1, &platformvertexbuffer, &sizeofvertexstuff, &nextpointinmemory);

	context->DrawIndexed(6, 0, 0);
}
void Platform::InitializePlatformTexture()
{
	hr = CreateDDSTextureFromFile(device, L"grass.dds", NULL, &PlatformShaderResourceView_textureview);
	assert(hr == S_OK);
}
Platform::~Platform()
{
	Platformindexbuffer->Release();
	platformvertexbuffer->Release();
	platformconstantbufferobject->Release();
	platforminputlayout->Release();

	platformvertex_shader->Release();
	platformpixel_shader->Release();
	PlatformSamplerState->Release();
	PlatformShaderResourceView_textureview->Release();
}

void Platform::DefaultSamplerDesc(D3D11_SAMPLER_DESC &_desc)
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