#include "../FBX_Exporter/DLLTransit.h"
#include "RenderObjects.h"
#include "simplemath.h"
#include "XTime.h"
#include "Platform.h"
#include "Skybox.h"

IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView *backbuffer;    // global declaration

									   //unique
ID3D11InputLayout *pLayout;            // the pointer to the input layout
ID3D11VertexShader *pVS;               // the pointer to the vertex shader
ID3D11PixelShader *pPS;                // the pointer to the pixel shader
ID3D11Buffer *VertexBuffer;                // the pointer to the vertex buffer
ID3D11Buffer *ConstantBuffer;		// the pointer to the constant buffer

/////////////////////////////////////////////
SEND_TO_SCENE toSceneShader;
//ID3D11Buffer *constantSceneBuffer = nullptr;
DirectX::XMMATRIX worldTranslate;

#define BACKBUFFER_WIDTH	1280 // 1100
#define BACKBUFFER_HEIGHT	720 // 800

#define NEAR_PLANE			0.1f
#define FAR_PLANE			300.0f
#define FOV_DEGREE			65.0f

#define CAMERAMOVEMENTSPEED 0.5f

XTime						time;
POINT						currPoint;

#define LOGANOSCAMERA 0
#define TORONTOCAMERA 1

Platform platform;
Skybox skybox;

// Depth Stencil Stuff
ID3D11DepthStencilView			*depthStencilView = nullptr;
ID3D11DepthStencilState			*depthStencilState = nullptr;
ID3D11Texture2D					*depthStenciltexture = nullptr;
ID3D11Buffer					*constantSceneBuffer = nullptr;

/////////////////////////////////////////////


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


void CleanD3D(void);
void RenderFrame(void);
void InitGraphics(void);    // creates the shape to render
void InitPipeline(void);    // loads and prepares the shaders


void InitRenderOBjects(HWND hWnd, std::vector<VERTEX> vertesess, std::vector<VERTEXBone> VE);
void RenderRenderObjects(std::vector<BoneInfo> BoneStuff_);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void UpdatePOS();
void UpdateCamera();
void CameraUPdadte();
void InitCamera();
/////////////////////////////////// Toronto Functions
void InitializeScene();
void DrawObjects();
void CreateDepthStencil();
void SetRenderTargets();


void KeyboardFunctions();
void MouseFunctions();
void UpdateviewMatrix();
///////////////////////////////////

void FBXRun(std::vector<VertexInfo> &returnData, std::vector<BoneInfo> &returnBone, Animation* animation)
{
	EXP::DLLTransit LoadStuffOne;
	std::string fileOne("..\\AnimatedAssests\\AnimatedBox\\Box_Walk.fbx");
	std::string binSaveOneLocation("fbx.bin");

	//	std::vector<VertexInfo> VertStuff;

	LoadStuffOne.saveFiletoBin(fileOne.c_str(), binSaveOneLocation.c_str());

	LoadStuffOne.loadFilefromBin(binSaveOneLocation.c_str(), returnData, returnBone, animation);
}




////////render objects stuff
RenderObjects Panel;
RenderObjects Box;
///////









//export stuff

//



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
	wc.lpszClassName = "WindowClass1";

	RegisterClassEx(&wc);
	RECT wr = { (long)0, (long)0, (long)BACKBUFFER_WIDTH, (long)BACKBUFFER_HEIGHT };
	//SCREEN SIZE
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindowEx(NULL,
		"WindowClass1",
		"DeadMAn",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(wr.right - wr.left),
		wr.bottom - wr.top,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);


	//loading in model






	std::vector<VertexInfo> VertStuff;
	std::vector<BoneInfo> BoneStuff;
	Animation* animation = nullptr;
	FBXRun(VertStuff, BoneStuff, animation);


	std::vector<VERTEX> Verts;
	std::vector<VERTEXBone> ves;

	for (unsigned i = 0; i < VertStuff.size(); i++)
	{
		VERTEX temp;
		temp.X = VertStuff[i].vert.x ;
		temp.Y = VertStuff[i].vert.y;
		temp.Z = VertStuff[i].vert.z;

		float col[4] = { 1.0f,0.2f , 0.0f, 1.0f };
		temp.Color[0] = col[0];
		temp.Color[1] = col[1];
		temp.Color[2] = col[2];
		temp.Color[3] = col[3];

		Verts.push_back(temp);
	}

	///////////////
	for (unsigned i = 0; i < Verts.size(); i++)
	{
		VERTEXBone Temp;
		Temp.X = Verts[i].X;
		Temp.Y = Verts[i].Y;
		Temp.Z = Verts[i].Z;
		Temp.Color[0] = Verts[i].Color[0];
		Temp.Color[1] = Verts[i].Color[1];
		Temp.Color[2] = Verts[i].Color[2];
		Temp.Color[3] = Verts[i].Color[3];
		unsigned Blendsize = VertStuff[i].blendWeights.size();
		switch (Blendsize)
		{
		case 1:
		{
			Temp.Weight[0] = VertStuff[i].blendWeights[0];
			Temp.Ind[0] = VertStuff[i].boneIndices[0];
			Temp.Weight[1] = 0;
			Temp.Ind[1] = 0;
			Temp.Weight[2] = 0;
			Temp.Ind[2] = 0;
			Temp.Weight[3] = 0;
			Temp.Ind[3] = 0;
		}
			break;
		case 2:
		{
			Temp.Weight[0] = VertStuff[i].blendWeights[0];
			Temp.Weight[1] = VertStuff[i].blendWeights[1];
			Temp.Ind[0] = VertStuff[i].boneIndices[0];
			Temp.Ind[1] = VertStuff[i].boneIndices[1];
			Temp.Weight[2] = 0;
			Temp.Ind[2] = 0;
			Temp.Weight[3] = 0;
			Temp.Ind[3] = 0;
		}
		break;
		case 3:
		{
			Temp.Weight[0] = VertStuff[i].blendWeights[0];
			Temp.Weight[1] = VertStuff[i].blendWeights[1];
			Temp.Weight[2] = VertStuff[i].blendWeights[2];
			Temp.Ind[0] = VertStuff[i].boneIndices[0];
			Temp.Ind[1] = VertStuff[i].boneIndices[1];
			Temp.Ind[2] = VertStuff[i].boneIndices[2];
			Temp.Weight[3] = 0;
			Temp.Ind[3] = 0;
		}
		break;
		case 4:
		{
			Temp.Weight[0] = VertStuff[i].blendWeights[0];
			Temp.Weight[1] = VertStuff[i].blendWeights[1];
			Temp.Weight[2] = VertStuff[i].blendWeights[2];
			Temp.Weight[3] = VertStuff[i].blendWeights[3];
			Temp.Ind[0] = VertStuff[i].boneIndices[0];
			Temp.Ind[1] = VertStuff[i].boneIndices[1];
			Temp.Ind[2] = VertStuff[i].boneIndices[2];
			Temp.Ind[3] = VertStuff[i].boneIndices[3];
		}
		break;
		default:
			break;
		}

		ves.push_back(Temp);
	}

	InitRenderOBjects(hWnd, Verts,ves);
	//InitD3D(hWnd);

#if	LOGANOSCAMERA
	InitCamera();
#elif TORONTOCAMERA
	InitializeScene();
#endif
	//	MSG msg = { 0 };
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
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
			//RenderFrame();
			RenderRenderObjects(BoneStuff);
		}
	}
	Panel.Clean();
	Box.Clean();
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

//void InitD3D(HWND hWnd)
//{
//	// create a struct to hold information about the swap chain
//	DXGI_SWAP_CHAIN_DESC scd;
//
//	// clear out the struct for use
//	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
//
//	// fill the swap chain description struct
//	scd.BufferCount = 1;                                    // one back buffer
//	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
//	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
//	scd.OutputWindow = hWnd;                                // the window to be used
//	scd.SampleDesc.Count = 4;                               // how many multisamples
//	scd.Windowed = TRUE;                                    // windowed/full-screen mode
//
//															// create a device, device context and swap chain using the information in the scd struct
//	D3D11CreateDeviceAndSwapChain(NULL,
//		D3D_DRIVER_TYPE_HARDWARE,
//		NULL,
//		NULL,
//		NULL,
//		NULL,
//		D3D11_SDK_VERSION,
//		&scd,
//		&swapchain,
//		&dev,
//		NULL,
//		&devcon);
//
//
//	// get the address of the back buffer
//	ID3D11Texture2D *pBackBuffer;
//	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
//
//	// use the back buffer address to create the render target
//	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
//	pBackBuffer->Release();
//
//	// set the render target as the back buffer
//	devcon->OMSetRenderTargets(1, &backbuffer, NULL);
//	devcon->OMSetDepthStencilState(depthStencilState, 0);
//
//	// Set the viewport
//	D3D11_VIEWPORT viewport;
//	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
//
//	viewport.TopLeftX = 0;
//	viewport.TopLeftY = 0;
//	viewport.Width = BACKBUFFER_WIDTH;
//	viewport.Height = BACKBUFFER_HEIGHT;
//
//	devcon->RSSetViewports(1, &viewport);
//
//
//	InitPipeline();
//	InitGraphics();
//}

void InitRenderOBjects(HWND hWnd, std::vector<VERTEX> vertesess, std::vector<VERTEXBone> VE) {
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
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

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

//	CreateDepthStencil();
	// set the render target as the back buffer


	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MaxDepth = 1;
	viewport.MinDepth = 0;
	viewport.Width = BACKBUFFER_WIDTH;
	viewport.Height = BACKBUFFER_HEIGHT;

	devcon->RSSetViewports(1, &viewport);

	CreateDepthStencil();

	devcon->OMSetRenderTargets(1, &backbuffer, depthStencilView);

	devcon->OMSetDepthStencilState(depthStencilState, 0);//

	Panel.InitEverythingBones(dev, devcon, VE);
	for (unsigned i = 0; i < vertesess.size(); i++)
	{

		vertesess[i].X *= 0.1f;
		vertesess[i].Y *= 0.1f;
		vertesess[i].Z *= 0.1f;
		vertesess[i].Color[0] = 0.1f;
		vertesess[i].Color[1] = 0.1f;
		vertesess[i].Color[2] = 0.76f;
		vertesess[i].Color[3] = 1.0f;
	}
	Box.InitEverything(dev, devcon, vertesess);
}


void DrawBox(DirectX::XMFLOAT4X4 position)
{
	devcon->OMSetRenderTargets(1, &backbuffer, NULL);

	ModelViewProjectionConstantBuffer lcb;
	//DirectX::XMStoreFloat4x4(&lcb.model, position);
	lcb.model = position;
	lcb.projection = proj;
	lcb.view = view;
	Box.Render(lcb, devcon);
}

bool Updateovertime = true;
unsigned frame = 0;
//struct KeyFrame
//{
//	float keyTime;
//	Float4x4 transform;
//};
float DeltaChange = 0;
void UpdateFrameTime(std::vector<BoneInfo> BoneStuff_)
{
	if (Updateovertime)
	{
		//todo: update with interperlation
		KeyFrame* Tempkey = BoneStuff_[0].keyframes->at(frame);
		float update = (*Tempkey).keyTime;

		DeltaChange += (float)time.Delta();
		if (DeltaChange >= update / ((float)frame + 1.0f))
		{
			frame += 1;
			frame = frame % 31;
			DeltaChange = 0;
		}

	}
	
}

void RenderRenderObjects(std::vector<BoneInfo> BoneStuff_) {





	float Color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	// clear the back buffer to a deep blue
	devcon->ClearRenderTargetView(backbuffer, Color);
	devcon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

#if	LOGANOSCAMERA
	CameraUPdadte();
#elif TORONTOCAMERA
	time.Signal();
	KeyboardFunctions();
	MouseFunctions();
#endif
	RenderObjects::FourBones BonesForConstantBuffer;
	DrawObjects();
	UpdateFrameTime(BoneStuff_);
	for (unsigned h = 0; h < BoneStuff_.size(); h++)
	{
		KeyFrame* Tempkey = BoneStuff_[h].keyframes->at(frame);
		
		DirectX::XMFLOAT4X4 Temp;
		for (unsigned i = 0; i < 4; i++)
		{
			for (unsigned k = 0; k < 4; k++)
			{

				if (i == 3 && k == 1)
				{
					Temp.m[i][k] =   (*Tempkey).transform.m[i][k];
					//Temp.m[i][k] = -1.0f * BoneStuff_[h].transform.m[i][k];
				}
				else
				{
					Temp.m[i][k] = (*Tempkey).transform.m[i][k];
					//Temp.m[i][k] = BoneStuff_[h].transform.m[i][k];
				}

			}
		}
		DrawBox(Temp);
		BonesForConstantBuffer.Bones[h] = Temp;
	}



	ModelViewProjectionConstantBuffer lcb;
	DirectX::XMStoreFloat4x4(&lcb.model, DirectX::XMMatrixIdentity());
	lcb.projection = proj;
	lcb.view = view;
	// make frame motion toggleable
	// make a read me



	Panel.Render(lcb, devcon,BonesForConstantBuffer);

	swapchain->Present(0, 0);

};

void CleanD3D()
{
	// close and release all existing COM objects
	swapchain->Release();
	dev->Release();
	devcon->Release();
	backbuffer->Release();

	/*pLayout->Release();
	pVS->Release();
	pPS->Release();
	VertexBuffer->Release();
	ConstantBuffer->Release();*/
}


#pragma region Legacy


void RenderFrame(void)
{


	float Color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	// clear the back buffer to a deep blue
	devcon->ClearRenderTargetView(backbuffer, Color);

	CameraUPdadte();

	ModelViewProjectionConstantBuffer cb;
	DirectX::XMStoreFloat4x4(&cb.model, DirectX::XMMatrixIdentity());
	cb.projection = proj;
	cb.view = view;


	D3D11_MAPPED_SUBRESOURCE _PVWResourse;
	devcon->Map(ConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &_PVWResourse);
	memcpy(_PVWResourse.pData, &cb, sizeof(ModelViewProjectionConstantBuffer));
	devcon->Unmap(ConstantBuffer, NULL);
	devcon->VSSetConstantBuffers(0, 1, &ConstantBuffer);

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);


	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	devcon->Draw(6, 0);


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
		{ 0.0f, -0.5f, 0.0f,{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ 1.0f, -0.5f, 1.0f,{ 0.0f, 0.0f, 1.0f, 1.0f } },
		{ 1.0f, -0.5f, 0.0f,{ 0.0f, 1.0f, 0.0f, 1.0f } },
		{ 0.0f, -0.5f, 0.0f,{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ 0.0f, -0.5f, 1.0f,{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ 1.0f, -0.5f, 1.0f,{ 0.0f, 0.0f, 1.0f, 1.0f } }


	};


	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * 6;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	dev->CreateBuffer(&bd, NULL, &VertexBuffer);       // create the buffer


													   // copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	devcon->Map(VertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
	devcon->Unmap(VertexBuffer, NULL);                                      // unmap the buffer
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;											// write access access by CPU and GPU
	bd.ByteWidth = sizeof(ModelViewProjectionConstantBuffer);             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;								 // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							// allow CPU to write in buffer

	dev->CreateBuffer(&bd, NULL, &ConstantBuffer);						// create the buffer
}
#pragma endregion



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
	DirectX::XMVECTOR transformedReference = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&avatarPosition), rotationMatrix);

	DirectX::XMStoreFloat3(&debug, transformedReference);
	float Debug1[3] = { debug.x, debug.y, debug.z };

	// Calculate the position the camera is looking at.
	DirectX::XMVECTOR cameraLookat = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&cameraPosition), transformedReference);



	DirectX::XMStoreFloat3(&debug, cameraLookat);
	float Debug2[3] = { debug.x, debug.y, debug.z };

	// Set up the view matrix and projection matrix.

	DirectX::XMVECTOR UP = { 0.0f, 1.0f, 0.0f };
	DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&cameraPosition), cameraLookat, UP));



}

void InitCamera()
{
	DirectX::XMFLOAT3 debug;
	// Calculate the camera's current position.

	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(avatarYaw);

	// Create a vector pointing the direction the camera is facing.
	DirectX::XMVECTOR transformedReference = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&cameraReference), rotationMatrix);

	DirectX::XMStoreFloat3(&debug, transformedReference);
	float Debug1[3] = { debug.x, debug.y, debug.z };


	// Calculate the position the camera is looking at.
	DirectX::XMVECTOR cameraLookat = { 0.5f,-0.6f, -0.5f };

	DirectX::XMStoreFloat3(&debug, cameraLookat);
	float Debug2[3] = { debug.x, debug.y, debug.z };

	// Set up the view matrix and projection matrix.

	DirectX::XMVECTOR UP = { 0.0f, 1.0f, 0.0f };
	DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&cameraPosition), cameraLookat, UP));

	//DirectX::XMLoadFloat4x4(&view)
	//DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationX((float)cos((double)45.0f)), DirectX::XMLoadFloat4x4(&view)));

	DirectX::XMStoreFloat4x4(&proj, DirectX::XMMatrixPerspectiveFovLH(viewAngle, ((float)BACKBUFFER_HEIGHT / (float)BACKBUFFER_WIDTH), nearClip, farClip));
}

void CameraUPdadte()
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
									 //////////////////////////////////////////////////
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


	if (GetAsyncKeyState(VK_RETURN) & 1)// enter key - toggle wire frame
	{
		Panel.ToggleWireFrame();
	}


}

#if TORONTOCAMERA
void InitializeScene()
{
	HRESULT hr;
	D3D11_BUFFER_DESC constscenedesc;
	ZeroMemory(&constscenedesc, sizeof(D3D11_BUFFER_DESC));

	constscenedesc.ByteWidth = sizeof(SEND_TO_SCENE);
	constscenedesc.Usage = D3D11_USAGE_DYNAMIC;
	constscenedesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constscenedesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	constscenedesc.MiscFlags = NULL;
	constscenedesc.StructureByteStride = NULL;
	hr = dev->CreateBuffer(&constscenedesc, NULL, &constantSceneBuffer);
	assert(hr == S_OK);



	ZeroMemory(&toSceneShader, sizeof(SEND_TO_SCENE));
	ZeroMemory(&worldTranslate, sizeof(DirectX::XMMATRIX));

	worldTranslate = DirectX::XMMatrixIdentity();

	toSceneShader.viewMatrix = DirectX::XMMatrixIdentity();
	
	toSceneShader.viewMatrix.r[3].m128_f32[1] += 2.5;
	toSceneShader.viewMatrix.r[3].m128_f32[2] -= 5.5;


	toSceneShader.viewMatrix = XMMatrixInverse(0, toSceneShader.viewMatrix);

	//	toSceneShader.projectionMatrix = XMMatrixProjection
	GetProjectionMatrix_4X4(FOV_DEGREE, NEAR_PLANE, FAR_PLANE, ((float)BACKBUFFER_HEIGHT / (float)BACKBUFFER_WIDTH), toSceneShader.projectionMatrix);
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			proj.m[i][j] = toSceneShader.projectionMatrix.r[i].m128_f32[j];
			view.m[i][j] = toSceneShader.viewMatrix.r[i].m128_f32[j];
		}
	}
	platform.Initialize(dev);
	skybox.Initialize(dev, devcon, &toSceneShader);
}
void KeyboardFunctions()
{
	float _time = (float)time.Delta();

	float _w = toSceneShader.viewMatrix.r[3].m128_f32[2];
	if (!_w)
		_w = 0.000001f;

	if (GetAsyncKeyState('S'))	// G
	{
		toSceneShader.viewMatrix.r[3].m128_f32[2] += 3 * _time * CAMERAMOVEMENTSPEED;
	}
	if (GetAsyncKeyState('W') || GetAsyncKeyState(VK_SPACE))  // T
	{
		toSceneShader.viewMatrix.r[3].m128_f32[2] -= 3 * _time * CAMERAMOVEMENTSPEED;
	}
	if (GetAsyncKeyState('D'))
	{
		toSceneShader.viewMatrix.r[3].m128_f32[0] -= 3 * _time * CAMERAMOVEMENTSPEED;
	}
	if (GetAsyncKeyState('A'))
	{
		toSceneShader.viewMatrix.r[3].m128_f32[0] += 3 * _time * CAMERAMOVEMENTSPEED;
	}
	if (GetAsyncKeyState(VK_RETURN) & 1)// enter key - toggle wire frame
	{
		Panel.ToggleWireFrame();
	}
	if (GetAsyncKeyState(VK_RIGHT) & 1)
	{
		frame += 1;
		frame = frame % 31;
	}
	UpdateviewMatrix();
	if (GetAsyncKeyState(VK_ESCAPE) & 1)// BackSpace - toggle wire frame
	{
		Updateovertime = !Updateovertime;
	}
}
void MouseFunctions()
{
	//	float _time = (float)time.Delta();
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		// Take Invverse
		toSceneShader.viewMatrix = XMMatrixInverse(0, toSceneShader.viewMatrix);

		// Save Position
		DirectX::XMMATRIX StoredMatrix = toSceneShader.viewMatrix;

		// Place Positions at 0
		for (int i = 0; i < 3; i++)
		{
			toSceneShader.viewMatrix.r[3].m128_f32[i] = 0;
		}
		POINT NewPoint;

		DirectX::XMMATRIX X_AxisRotation = DirectX::XMMatrixRotationX(0);
		DirectX::XMMATRIX Y_AxisRotation = DirectX::XMMatrixRotationY(0);

		if (GetCursorPos(&NewPoint))
		{
			X_AxisRotation = DirectX::XMMatrixRotationX((NewPoint.y - currPoint.y) * .01f);
			Y_AxisRotation = DirectX::XMMatrixRotationY((NewPoint.x - currPoint.x) * .01f);

			currPoint = NewPoint;
		}

		// Multiply Matricies in Reverse Order
		toSceneShader.viewMatrix = XMMatrixMultiply(toSceneShader.viewMatrix, Y_AxisRotation);
		toSceneShader.viewMatrix = XMMatrixMultiply(X_AxisRotation, toSceneShader.viewMatrix);

		XMMatrixMultiply(toSceneShader.viewMatrix, Y_AxisRotation);
		toSceneShader.viewMatrix = XMMatrixMultiply(X_AxisRotation, toSceneShader.viewMatrix);

		// Restore original position
		toSceneShader.viewMatrix.r[3].m128_f32[0] = StoredMatrix.r[3].m128_f32[0];
		toSceneShader.viewMatrix.r[3].m128_f32[1] = StoredMatrix.r[3].m128_f32[1];
		toSceneShader.viewMatrix.r[3].m128_f32[2] = StoredMatrix.r[3].m128_f32[2];


		// Inverse it again
		toSceneShader.viewMatrix = XMMatrixInverse(0, toSceneShader.viewMatrix);

	}
	else
		GetCursorPos(&currPoint);
	UpdateviewMatrix();
}
void UpdateviewMatrix()
{
	XMStoreFloat4x4(&view, toSceneShader.viewMatrix);
}
void CreateDepthStencil()
{
	D3D11_TEXTURE2D_DESC texturedesc;
	ZeroMemory(&texturedesc, sizeof(D3D11_TEXTURE2D_DESC));

	texturedesc.Width = BACKBUFFER_WIDTH;
	texturedesc.Height = BACKBUFFER_HEIGHT;
	texturedesc.MipLevels = 1;
	texturedesc.ArraySize = 1;
	texturedesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texturedesc.SampleDesc.Count = 1;
	texturedesc.SampleDesc.Quality = 0;
	texturedesc.Usage = D3D11_USAGE_DEFAULT;
	texturedesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texturedesc.CPUAccessFlags = 0;
	texturedesc.MiscFlags = 0;

	HRESULT	hr;

	hr = dev->CreateTexture2D(&texturedesc, NULL, &depthStenciltexture);
	assert(hr == S_OK);

	hr = dev->CreateDepthStencilView(depthStenciltexture, NULL, &depthStencilView);
	assert(hr == S_OK);
}
void SetRenderTargets()
{
//	devcon->OMSetRenderTargets(1, &RTV, depthStencilView);
	devcon->OMSetDepthStencilState(depthStencilState, 0);
}



void DrawObjects()
{
	HRESULT hr;

	skybox.Draw();

//	devcon->OMSetDepthStencilState(depthStencilState, 0);
	devcon->VSSetConstantBuffers(1, 1, &constantSceneBuffer);

	D3D11_MAPPED_SUBRESOURCE mapScene;
	hr = devcon->Map(constantSceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapScene);
	assert(hr == S_OK);

	memcpy(mapScene.pData, &toSceneShader, sizeof(SEND_TO_SCENE));
	devcon->Unmap(constantSceneBuffer, 0);

	platform.Draw(devcon);
}
#endif
