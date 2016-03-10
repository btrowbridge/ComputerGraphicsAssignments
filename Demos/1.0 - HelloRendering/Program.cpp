#include "pch.h"

using namespace std;
using namespace DirectX;
using namespace Library;

void InitializeWindow(HINSTANCE instance, const wstring& className, const wstring windowTitle, int showCommand);
LRESULT WINAPI WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
POINT CenterWindow(int windowWidth, int windowHeight);
void InitializeDirectX();
void Shutdown(const wstring& className);

const UINT mScreenWidth = 1024;
const UINT mScreenHeight = 768;

HWND mWindowHandle;
WNDCLASSEX mWindow;

D3D_FEATURE_LEVEL mFeatureLevel;
ID3D11Device1* mDirect3DDevice;
ID3D11DeviceContext1* mDirect3DDeviceContext;
IDXGISwapChain1* mSwapChain;
ID3D11RenderTargetView* mRenderTargetView;
ID3D11DepthStencilView* mDepthStencilView;

const XMVECTORF32 BackgroundColor = { 0.392f, 0.584f, 0.929f, 1.0f };

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
{
	UNREFERENCED_PARAMETER(previousInstance);
	UNREFERENCED_PARAMETER(commandLine);

	ThrowIfFailed(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED), "Error initializing COM.");

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	wstring windowClassName = L"RenderingClass";
	InitializeWindow(instance, windowClassName, L"Hello, Rendering!", showCommand);
	InitializeDirectX();

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
			mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&BackgroundColor));
			mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			// Game-specific code

			ThrowIfFailed(mSwapChain->Present(0, 0), "IDXGISwapChain::Present() failed.");
		}
	}

	Shutdown(windowClassName);
	CoUninitialize();

	return static_cast<int>(message.wParam);
}

void InitializeWindow(HINSTANCE instance, const wstring& className, const wstring windowTitle, int showCommand)
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

	RECT windowRectangle = { 0, 0, mScreenWidth, mScreenHeight };
	AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);

	RegisterClassEx(&mWindow);
	POINT center = CenterWindow(mScreenWidth, mScreenHeight);
	mWindowHandle = CreateWindow(className.c_str(), windowTitle.c_str(), WS_OVERLAPPEDWINDOW, center.x, center.y, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, nullptr, nullptr, instance, nullptr);

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

#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* direct3DDevice = nullptr;
	ID3D11DeviceContext* direct3DDeviceContext = nullptr;
	ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &direct3DDevice, &mFeatureLevel, &direct3DDeviceContext), "D3D11CreateDevice() failed");
	ThrowIfFailed(direct3DDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&mDirect3DDevice)), "ID3D11Device::QueryInterface() failed");
	ThrowIfFailed(direct3DDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&mDirect3DDeviceContext)), "ID3D11Device::QueryInterface() failed");

	ReleaseObject(direct3DDevice);
	ReleaseObject(direct3DDeviceContext);

	UINT multiSamplingCount = 4;
	UINT multiSamplingQualityLevels;
	ThrowIfFailed(mDirect3DDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, multiSamplingCount, &multiSamplingQualityLevels), "CheckMultisampleQualityLevels() failed.");
	if (multiSamplingQualityLevels == 0)
	{
		throw GameException("Unsupported multi-sampling quality");
	}

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.Width = mScreenWidth;
	swapChainDesc.Height = mScreenHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = multiSamplingCount;
	swapChainDesc.SampleDesc.Quality = multiSamplingQualityLevels - 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	IDXGIDevice* dxgiDevice = nullptr;
	ThrowIfFailed(mDirect3DDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice)), "ID3D11Device::QueryInterface() failed");

	IDXGIAdapter *dxgiAdapter = nullptr;
	HRESULT hr;
	if (FAILED(hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter))))
	{
		ReleaseObject(dxgiDevice);
		throw GameException("IDXGIDevice::GetParent() failed retrieving adapter.", hr);
	}

	IDXGIFactory2* dxgiFactory = nullptr;
	if (FAILED(hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory))))
	{
		ReleaseObject(dxgiDevice);
		ReleaseObject(dxgiAdapter);
		throw GameException("IDXGIAdapter::GetParent() failed retrieving factory.", hr);
	}

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = { 0 };
	fullScreenDesc.RefreshRate.Numerator = 60;
	fullScreenDesc.RefreshRate.Denominator = 1;
	fullScreenDesc.Windowed = true;

	if (FAILED(hr = dxgiFactory->CreateSwapChainForHwnd(dxgiDevice, mWindowHandle, &swapChainDesc, &fullScreenDesc, nullptr, &mSwapChain)))
	{
		ReleaseObject(dxgiDevice);
		ReleaseObject(dxgiAdapter);
		ReleaseObject(dxgiFactory);
		throw GameException("IDXGIDevice::CreateSwapChainForHwnd() failed.", hr);
	}

	ReleaseObject(dxgiDevice);
	ReleaseObject(dxgiAdapter);
	ReleaseObject(dxgiFactory);

	ID3D11Texture2D* backBuffer;
	ThrowIfFailed(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)), "IDXGISwapChain::GetBuffer() failed.");
	if (FAILED(hr = mDirect3DDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView)))
	{
		ReleaseObject(backBuffer);
		throw GameException("IDXGIDevice::CreateRenderTargetView() failed.", hr);
	}

	ReleaseObject(backBuffer);

	D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
	depthStencilDesc.Width = mScreenWidth;
	depthStencilDesc.Height = mScreenHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.SampleDesc.Count = multiSamplingCount;
	depthStencilDesc.SampleDesc.Quality = multiSamplingQualityLevels - 1;

	ID3D11Texture2D* depthStencilBuffer;
	ThrowIfFailed(mDirect3DDevice->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer), "IDXGIDevice::CreateTexture2D() failed.");
	ThrowIfFailed(mDirect3DDevice->CreateDepthStencilView(depthStencilBuffer, nullptr, &mDepthStencilView), "IDXGIDevice::CreateDepthStencilView() failed.");
	ReleaseObject(depthStencilBuffer);

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(mScreenWidth);
	viewport.Height = static_cast<float>(mScreenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	mDirect3DDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	mDirect3DDeviceContext->RSSetViewports(1, &viewport);
}

void Shutdown(const wstring& className)
{
	if (mDirect3DDeviceContext != nullptr)
	{
		mDirect3DDeviceContext->ClearState();
		mDirect3DDeviceContext->Flush();
	}

	ReleaseObject(mRenderTargetView);
	ReleaseObject(mDepthStencilView);
	ReleaseObject(mSwapChain);	
	ReleaseObject(mDirect3DDeviceContext);
	ReleaseObject(mDirect3DDevice);

	UnregisterClass(className.c_str(), mWindow.hInstance);
}