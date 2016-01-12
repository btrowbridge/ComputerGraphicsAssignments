
#include "pch.h"

using namespace std;

HWND mWindowHandle;
WNDCLASSEX mWindow;

const UINT mScreenHight = 1024;
const UINT mScreenWidth = 768;


void InitializeWindow(HINSTANCE instance, const wstring& className, const wstring& windowTitle, int showCommand);
LRESULT WINAPI WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
POINT CenterWindow(int windowWidth, int windowHeight);


int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
{
	UNREFERENCED_PARAMETER(previousInstance);
	UNREFERENCED_PARAMETER(commandLine);

	wstring className = L"RendereingClass";

	InitializeWindow(instance, className, L"Hello, Rendering!", showCommand);

	MSG message = { 0 };

	while (message.message != WM_QUIT) 
	{
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else 
		{
			//Game Specific code
		}
	}
	return 0;
}

void InitializeWindow(HINSTANCE instance, const wstring& className, const wstring& windowTitle, int showCommand)
{
	ZeroMemory(&mWindow, sizeof(mWindow));
	mWindow.cbSize = sizeof(WNDCLASSEX);
	mWindow.style = CS_CLASSDC;
	mWindow.lpfnWndProc = WndProc;
	mWindow.hInstance = instance;
	mWindow.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	mWindow.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	mWindow.hCursor = LoadCursor(nullptr, IDC_ARROW);
	mWindow.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
	mWindow.lpszClassName = className.c_str();

	RECT windowRectangle = { 0, 0, mScreenWidth, mScreenHight };
	AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);

	RegisterClassEx(&mWindow);
	POINT center = CenterWindow(mScreenWidth, mScreenHight);
	mWindowHandle = CreateWindow(className.c_str(), windowTitle.c_str(), WS_OVERLAPPEDWINDOW, 
		center.x, center.y, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, nullptr,nullptr, instance, nullptr);

	ShowWindow(mWindowHandle, showCommand);
	UpdateWindow(mWindowHandle);


}

LRESULT WINAPI WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(windowHandle, message, wParam, lParam);
}

POINT CenterWindow(int windowWidth, int windowHeight)
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	POINT center;
	center.x = (screenWidth - windowWidth) / 2;
	center.y = (screenHeight - windowHeight) / 2;

	return center;
}

void InitializeDirectX()
{
	UINT createDeviceFlags = 0;

#if defined(Debug) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG; //Debug or no?
#endif
	//DirectX version supported levels
	D3D_FEATURE_LEVEL featuredLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	//Graphics Device
	ID3D11Device* direct3DDevice = nullptr;
	ID3D11DeviceContext* direct3DDeviceContext = nullptr;
	HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featuredLevels, ARRAYSIZE(featuredLevels), UINT D3D10_SDK_VERSION, &direct3DDevice, &mFeatureLevels /*Outside feature level*/, &direct3DDeviceContext);
	
	if (FAILED(hr))
	{
		throw exception("D3D11CreateDevice(...) failed. Line: 112");
	}
	
}


