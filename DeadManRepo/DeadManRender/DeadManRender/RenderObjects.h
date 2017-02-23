#pragma once
#pragma comment (lib, "d3D11.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include <d3D11.h>
#include <directxmath.h>
#include <vector>
#include <Windows.h>
#include <d3D11.h>
#include <directxmath.h>
#include <vector>
#include "TVS.csh"
#include "TPS.csh"
//#include "simplemath.h"


////////structs
struct VERTEX { FLOAT X, Y, Z; float Color[4]; };  // a struct to define a single vertex

struct ModelViewProjectionConstantBuffer
{
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

////////

class RenderObjects
{
public:
	RenderObjects();
	~RenderObjects();

	unsigned size = 0;
	ID3D11InputLayout *Layout;            // the pointer to the input layout
	ID3D11VertexShader *VS;               // the pointer to the vertex shader
	ID3D11PixelShader *PS;                // the pointer to the pixel shader
	ID3D11Buffer *_VertexBuffer;                // the pointer to the vertex buffer
	ID3D11Buffer *_ConstantBuffer;		// the pointer to the constant buffer

	ID3D11RasterizerState *_RSwire; //the pointer to the raster state
	ID3D11RasterizerState *_RSsolid; //the pointer to the raster state


	bool wire = false;




	void Clean() {
		Layout->Release();
		VS->Release();
		PS->Release();
		_VertexBuffer->Release();
		_ConstantBuffer->Release();
		_RSwire->Release();
		_RSsolid->Release();
	}
	void UpdateStuff(ModelViewProjectionConstantBuffer &cbp, ID3D11DeviceContext* _devcon)
	{
		D3D11_MAPPED_SUBRESOURCE _PVWResourse;
		_devcon->Map(_ConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &_PVWResourse);
		memcpy(_PVWResourse.pData, &cbp, sizeof(ModelViewProjectionConstantBuffer));
		_devcon->Unmap(_ConstantBuffer, NULL);
	}
	void SetStuff(ID3D11DeviceContext* _devcon)
	{
		_devcon->VSSetConstantBuffers(0, 1, &_ConstantBuffer);

		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		_devcon->IASetVertexBuffers(0, 1, &_VertexBuffer, &stride, &offset);


		_devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		_devcon->VSSetShader(VS, 0, 0);
		_devcon->PSSetShader(PS, 0, 0);
		_devcon->IASetInputLayout(Layout);


		(wire == true) ? _devcon->RSSetState(_RSwire) : _devcon->RSSetState(_RSsolid);


	}
	void Render(ModelViewProjectionConstantBuffer &cb, ID3D11DeviceContext* _devcon)
	{
		//replace 6 with the size of the verts
		UpdateStuff(cb,_devcon);
		SetStuff(_devcon);
		_devcon->Draw(size, 0);
	}

	void InitShadders(ID3D11Device *dev)
	{
		dev->CreateVertexShader(TVS, sizeof(TVS), NULL, &VS);
		dev->CreatePixelShader(TPS, sizeof(TPS), NULL, &PS);
	}

	void InitInputLayout(ID3D11Device *dev)
	{
		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		dev->CreateInputLayout(ied, 2, TVS, sizeof(TVS), &Layout);
	}

	void InitVerts(ID3D11Device *dev, ID3D11DeviceContext* devcon, VERTEX* object, unsigned size_)
	{
		/*size = 6;
		VERTEX OurVertices[] =
		{
			{ 0.0f, -0.5f, 0.0f,{ 1.0f, 0.0f, 0.0f, 1.0f } },
			{ 1.0f, -0.5f, 1.0f,{ 0.0f, 0.0f, 1.0f, 1.0f } },
			{ 1.0f, -0.5f, 0.0f,{ 0.0f, 1.0f, 0.0f, 1.0f } },
			{ 0.0f, -0.5f, 0.0f,{ 1.0f, 0.0f, 0.0f, 1.0f } },
			{ 0.0f, -0.5f, 1.0f,{ 1.0f, 0.0f, 0.0f, 1.0f } },
			{ 1.0f, -0.5f, 1.0f,{ 0.0f, 0.0f, 1.0f, 1.0f } }


		};*/
		size = size_;

		// create the vertex buffer
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
		bd.ByteWidth = sizeof(VERTEX) * size;             // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

		dev->CreateBuffer(&bd, NULL, &_VertexBuffer);       // create the buffer

		D3D11_MAPPED_SUBRESOURCE ms;
		devcon->Map(_VertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
		memcpy(ms.pData, object, size*sizeof(VERTEX));                 // copy the data
		devcon->Unmap(_VertexBuffer, NULL);                                      // unmap the buffer
		ZeroMemory(&bd, sizeof(bd));
	}
	//VertStuff

	void InitVertsText(ID3D11Device *dev, ID3D11DeviceContext* devcon)
	{
		size = 3;
		VERTEX OurVertices[] =
		{
			{ -1.0f, -0.5f, -1.0f,{ 1.0f, 0.0f, 0.0f, 1.0f } },
			{ -2.0f, -0.5f, -2.0f,{ 0.0f, 0.0f, 1.0f, 1.0f } },
			{ -2.0f, -0.5f, -1.0f,{ 0.0f, 1.0f, 0.0f, 1.0f } }
			
		};


		// create the vertex buffer
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
		bd.ByteWidth = sizeof(VERTEX) * size;             // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

		dev->CreateBuffer(&bd, NULL, &_VertexBuffer);       // create the buffer

		D3D11_MAPPED_SUBRESOURCE ms;
		devcon->Map(_VertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
		memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
		devcon->Unmap(_VertexBuffer, NULL);                                      // unmap the buffer
		ZeroMemory(&bd, sizeof(bd));
	}


	void ToggleWireFrame()
	{
		wire = !wire;
	}



	void IntiConstantBuffer(ID3D11Device *dev)
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DYNAMIC;											// write access access by CPU and GPU
		bd.ByteWidth = sizeof(ModelViewProjectionConstantBuffer);             // size is the VERTEX struct * 3
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;								 // use as a vertex buffer
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							// allow CPU to write in buffer

		dev->CreateBuffer(&bd, NULL, &_ConstantBuffer);						// create the buffer
	}

	void InitEverything(ID3D11Device *kdev, ID3D11DeviceContext* kdevcon, std::vector<VERTEX> t)
	{
		InitShadders(kdev);
		InitInputLayout(kdev);
		InitVerts(kdev,kdevcon,t.data(),t.size());
		IntiConstantBuffer(kdev);

		D3D11_RASTERIZER_DESC Temp;
		ZeroMemory(&Temp, sizeof(Temp));
		Temp.CullMode = D3D11_CULL_BACK;
		Temp.FillMode = D3D11_FILL_SOLID;
		kdev->CreateRasterizerState(&Temp,&_RSsolid);
		ZeroMemory(&Temp, sizeof(Temp));
		Temp.CullMode = D3D11_CULL_BACK;
		Temp.FillMode = D3D11_FILL_WIREFRAME;
		kdev->CreateRasterizerState(&Temp, &_RSwire);
		wire = false;
	}
};

