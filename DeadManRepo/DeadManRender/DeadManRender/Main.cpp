#include <Windows.h>
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#define Width_W 1080
#define Height_W 720

#pragma region FowardDeclariations
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void Cleanup();
#pragma endregion


IDXGISwapChain *SwapChain;
ID3D11Device *Device;
ID3D11DeviceContext *DeviceContext;
ID3D11RenderTargetView* BackBuffer;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// set up window handler
	HWND WindowHandle;

	WNDCLASSEX WindowsClassDesc;
	ZeroMemory(&WindowsClassDesc, sizeof(WNDCLASSEX));
	WindowsClassDesc.cbSize = sizeof(WNDCLASSEX);
	WindowsClassDesc.style = CS_HREDRAW | CS_VREDRAW;
	WindowsClassDesc.lpfnWndProc = WindowProc;
	WindowsClassDesc.hInstance = hInstance;
	WindowsClassDesc.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowsClassDesc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	WindowsClassDesc.lpszClassName = "WindowClass1";
	RegisterClassEx(&WindowsClassDesc);
	// set up window size
	RECT WindowsRect = { 0, 0, Width_W, Height_W };
	AdjustWindowRect(&WindowsRect, WS_OVERLAPPEDWINDOW, FALSE);
	WindowHandle = CreateWindowEx(NULL, "WindowClass1", "DeadManRendering", WS_OVERLAPPEDWINDOW, 50, 50, WindowsRect.right - WindowsRect.left, WindowsRect.bottom - WindowsRect.top, NULL, NULL, hInstance, NULL);
	ShowWindow(WindowHandle, nShowCmd);

	// set up swapchain
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.OutputWindow = WindowHandle;
	SwapChainDesc.SampleDesc.Count = 4;
	SwapChainDesc.Windowed = TRUE;
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain, &Device, NULL, &DeviceContext);

	//set up back buffer
	ID3D11Texture2D *BackBufferTexture;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBufferTexture);
	Device->CreateRenderTargetView(BackBufferTexture, NULL, &BackBuffer);

	BackBufferTexture->Release();
	// set up viewport
	D3D11_VIEWPORT ViewPort;
	ZeroMemory(&ViewPort, sizeof(ViewPort));
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;
	ViewPort.Width = Width_W;
	ViewPort.Height = Height_W;
	DeviceContext->RSSetViewports(1, &ViewPort);
	
	


	MSG msg = { 0 };
	while (TRUE)
	{
		//clear Window to this color
		FLOAT RedBackGround[4] = { 1.0f,0.0f,0.0f,1.0f };
		DeviceContext->ClearRenderTargetView(BackBuffer, RedBackGround);


		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				break;
		}
		//update the camera

		//update all models/meshes/animations

		//draw all models/meshes/animations

		//present all models/meshes/animations
		SwapChain->Present(0, 0);


	}
	Cleanup();

}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Cleanup()
{
	SwapChain->Release();
	BackBuffer->Release();
	Device->Release();
	DeviceContext->Release();
}