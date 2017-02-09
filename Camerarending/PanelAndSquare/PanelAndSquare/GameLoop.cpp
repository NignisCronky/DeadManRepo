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


IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView *backbuffer;    // global declaration

//unique
ID3D11InputLayout *pLayout;            // the pointer to the input layout
ID3D11VertexShader *pVS;               // the pointer to the vertex shader
ID3D11PixelShader *pPS;                // the pointer to the pixel shader
ID3D11Buffer *pVBuffer;                // the pointer to the vertex buffer
ID3D11Buffer *ConstantBuffer;		// the pointer to the constant buffer


struct VERTEX { FLOAT X, Y, Z; float Color[4]; };  // a struct to define a single vertex


DirectX::XMFLOAT4X4 view;
DirectX::XMFLOAT4X4 proj;

DirectX::XMFLOAT3  avatarPosition = { 0, 0, -50 };
DirectX::XMFLOAT3 cameraPosition = avatarPosition;

float avatarYaw = 0;

DirectX::XMFLOAT3 cameraReference = { 0, 0, 1 };

float rotationSpeed = 1.0f / 60.0f;
float forwardSpeed = 50.0f / 60.0f;

float viewAngle = DirectX::XM_PI / 4.0f;

float nearClip = 1.0f;
float farClip = 2000.0f;





void InitD3D(HWND hWnd);
void CleanD3D(void);
void RenderFrame(void);
void InitGraphics(void);    // creates the shape to render
void InitPipeline(void);    // loads and prepares the shaders

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void UpdatePOS();
void UpdateCamera();
void CameraUPdadte();




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass1";

	RegisterClassEx(&wc);
	RECT wr = { (long)0, (long)0, (long)1100, (long)800 };
	//SCREEN SIZE
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindowEx(NULL,
		L"WindowClass1",
		L"DeadMAn",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(wr.right - wr.left),
		wr.bottom - wr.top,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);


	InitD3D(hWnd);



	MSG msg;
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}
		else
		{
			RenderFrame();
		}
	}
	CleanD3D();
	return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void InitD3D(HWND hWnd)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hWnd;                                // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

															// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		NULL,
		&devcon);


	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	// set the render target as the back buffer
	devcon->OMSetRenderTargets(1, &backbuffer, NULL);


	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 1100;
	viewport.Height = 800;

	devcon->RSSetViewports(1, &viewport);


	InitPipeline();
	InitGraphics();
}

void CleanD3D()
{
	// close and release all existing COM objects
	swapchain->Release();
	dev->Release();
	devcon->Release();
	backbuffer->Release();

	pLayout->Release();
	pVS->Release();
	pPS->Release();
	pVBuffer->Release();
	ConstantBuffer->Release();
}


struct ModelViewProjectionConstantBuffer
{
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

void RenderFrame(void)
{


	float Color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	// clear the back buffer to a deep blue
	devcon->ClearRenderTargetView(backbuffer, Color);



	UpdatePOS();
	UpdateCamera();
	CameraUPdadte();



	/////////////////////////
	// update costant beffer
	ModelViewProjectionConstantBuffer cb;

	
	DirectX::XMStoreFloat4x4(&cb.model, DirectX::XMMatrixIdentity());
	cb.projection = proj;
	cb.view = view;
	//devcon->UpdateSubresource(ConstantBuffer, 0, NULL, &cb, 0, 0);

	//devcon->VSSetConstantBuffers(0, 1, &ConstantBuffer);





	D3D11_MAPPED_SUBRESOURCE _PVWResourse;
	devcon->Map(ConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &_PVWResourse);
	memcpy(_PVWResourse.pData, &cb, sizeof(ModelViewProjectionConstantBuffer));
	devcon->Unmap(ConstantBuffer, NULL);
	devcon->VSSetConstantBuffers(0, 1, &ConstantBuffer);







	/////////////


	// select which vertex buffer to display
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

	// select which primtive type we are using
	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw the vertex buffer to the back buffer
	devcon->Draw(3, 0);

	// switch the back buffer and the front buffer
	swapchain->Present(0, 0);
}

void InitPipeline()
{

	// encapsulate both shaders into shader objects
	dev->CreateVertexShader(TVS, sizeof(TVS), NULL, &pVS);
	dev->CreatePixelShader(TPS, sizeof(TPS), NULL, &pPS);


	// set the shader objects
	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	dev->CreateInputLayout(ied, 2, TVS, sizeof(TVS), &pLayout);
	devcon->IASetInputLayout(pLayout);


}

void InitGraphics()
{
	// create a triangle using the VERTEX struct
	VERTEX OurVertices[] =
	{
		{ 0.0f, 0.5f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f} },
		{ 0.45f, -0.5, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f} },
		{ -0.45f, -0.5f, 0.0f,{0.0f, 0.0f, 1.0f, 1.0f} }
	};


	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	dev->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer


												   // copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
	devcon->Unmap(pVBuffer, NULL);                                      // unmap the buffer




	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;											// write access access by CPU and GPU
	bd.ByteWidth = sizeof(ModelViewProjectionConstantBuffer);             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;								 // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							// allow CPU to write in buffer

	dev->CreateBuffer(&bd, NULL, &ConstantBuffer);						// create the buffer
}





void UpdatePOS()
{
	if (GetAsyncKeyState(VK_LEFT))
	{
		// Rotate left.
		avatarYaw += rotationSpeed;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		// Rotate right.
		avatarYaw -= rotationSpeed;
	}
	if (GetAsyncKeyState(VK_UP))
	{
		DirectX::XMMATRIX forwardMovement = DirectX::XMMatrixRotationY(avatarYaw);
		DirectX::XMVECTOR v = { 0, 0, forwardSpeed };
		v = DirectX::XMVector3Transform(v, forwardMovement);
		DirectX::XMFLOAT3 Temp;
		DirectX::XMStoreFloat3(&Temp, v);
		avatarPosition.z += Temp.z;
		avatarPosition.x += Temp.x;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		DirectX::XMMATRIX forwardMovement = DirectX::XMMatrixRotationY(avatarYaw);
		DirectX::XMVECTOR v = { 0, 0, -forwardSpeed };
		v = DirectX::XMVector3Transform(v, forwardMovement);
		DirectX::XMFLOAT3 Temp;
		DirectX::XMStoreFloat3(&Temp, v);
		avatarPosition.z += Temp.z;
		avatarPosition.x += Temp.x;
	}
}

void UpdateCamera()
{
	DirectX::XMFLOAT3 debug;
	// Calculate the camera's current position.

	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(avatarYaw);

	// Create a vector pointing the direction the camera is facing.
	DirectX::XMVECTOR transformedReference = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&cameraReference), rotationMatrix);

	DirectX::XMStoreFloat3(&debug, transformedReference);
	float Debug1[3] = { debug.x, debug.y, debug.z };

	// Calculate the position the camera is looking at.
	DirectX::XMVECTOR cameraLookat = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&cameraPosition), transformedReference);



	DirectX::XMStoreFloat3(&debug, cameraLookat);
	float Debug2[3] = {debug.x, debug.y, debug.z};

	// Set up the view matrix and projection matrix.

	DirectX::XMVECTOR UP= { 0.0f, 1.0f, 0.0f };
	DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&cameraPosition), cameraLookat, UP));



	DirectX::XMStoreFloat4x4(&proj, DirectX::XMMatrixPerspectiveFovLH(viewAngle, (800.0f / 1100.0f), nearClip, farClip));
}


void CameraUPdadte()
{
	if (GetAsyncKeyState(0x57)) {//W - Move Forward
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, forwardSpeed);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&view, result);
	}
	if (GetAsyncKeyState(0x53)) {//S - Move Backward
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, -forwardSpeed);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&view, result);
	}
	if (GetAsyncKeyState(0x41)) {//A - Strafe Left
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(-forwardSpeed, 0.0f, 0.0f);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&view, result);
	}
	if (GetAsyncKeyState(0x44)) {//D - Strafe Right
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(forwardSpeed, 0.0f, 0.0f);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&view, result);
	}
	if (GetAsyncKeyState(VK_SPACE)) {//Space - Move Up
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, forwardSpeed, 0.0f);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&view, result);
	}
	if (GetAsyncKeyState(VK_LSHIFT)) {//Shift - Move Down
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, -forwardSpeed, 0.0f);
		DirectX::XMMATRIX temp_camera = DirectX::XMLoadFloat4x4(&view);
		DirectX::XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&view, result);
	}
}