
#include "pch.h"
#include "Game.h"

using namespace std;
using namespace Library;
using namespace DirectX;

void InitializeDirectX();
void InitializeWindow(HINSTANCE instance, const wstring& className, const wstring& windowTitle, int showCommand);
LRESULT WINAPI WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
POINT CenterWindow(int windowWidth, int windowHeight);
void ShutDown(const wstring& className);


HWND mWindowHandle;
WNDCLASSEX mWindow;

const UINT mScreenHight = 1024;
const UINT mScreenWidth = 768;

const UINT mFrameRate = 60;
bool mIsFullScreen = false;
bool mMultiSamplingEnabled;


D3D_FEATURE_LEVEL mFeatureLevel;
IDXGISwapChain1* mSwapChain;
ID3D11Device1* mDirect3DDevice;
ID3D11DeviceContext1* mDirect3DDeviceContext;
ID3D11RenderTargetView* mRenderTargetView;
ID3D11DepthStencilView* mDepthStencilView;


const XMVECTORF32 backgroundColor = { 0.0f, 0.750f, 1.0f, 1.0f }; //RGBA


int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
{
	UNREFERENCED_PARAMETER(previousInstance);
	UNREFERENCED_PARAMETER(commandLine);

	ThrowIfFailed(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED), "Error initializing COM");

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	wstring className = L"RendereingClass";

	InitializeWindow(instance, className, L"Hello, Rendering!", showCommand);
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
			mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&backgroundColor));
			mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f,0 );
			//Game Specific code
			ThrowIfFailed(mSwapChain->Present(1,0), "IDXGISwapChain::Present() Failed");
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
	mWindowHandle = CreateWindow(className.c_str(), windowTitle.c_str(), WS_OVERLAPPEDWINDOW, center.x, center.y, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, nullptr,nullptr, instance, nullptr);

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
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	//Graphics Device
	ID3D11Device* direct3DDevice = nullptr;
	ID3D11DeviceContext* direct3DDeviceContext = nullptr;
	
	
	ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &direct3DDevice, &mFeatureLevel, &direct3DDeviceContext), "D3D11CreateDevice(...) failed.Line: 112");

	ThrowIfFailed(direct3DDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&mDirect3DDevice)), "ID3D11Device::QueryInterface() failed");
	ThrowIfFailed(direct3DDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&mDirect3DDeviceContext)), "ID3D11Device::QueryInterface() failed");

	ReleaseObject(direct3DDevice);
	ReleaseObject(direct3DDeviceContext);

	//Populating a swap chain
	UINT mMultiSamplingCount = 4;
	UINT mMultiSamplingQualityLevels;
	ThrowIfFailed(mDirect3DDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, mMultiSamplingCount, &mMultiSamplingQualityLevels), "CheckMultisampleQualityLevels() failed.");

	if (mMultiSamplingQualityLevels == 0) {
		throw GameException("Unsupported multi-sampling quality");
	}

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	//ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.Width = mScreenWidth;
	swapChainDesc.Height = mScreenHight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (mMultiSamplingEnabled)
	{
		swapChainDesc.SampleDesc.Count = mMultiSamplingCount;
		swapChainDesc.SampleDesc.Quality = mMultiSamplingQualityLevels - 1;
	}
	else
	{
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = { 0 };
	ZeroMemory(&fullScreenDesc, sizeof(fullScreenDesc));
	fullScreenDesc.RefreshRate.Numerator = mFrameRate;
	fullScreenDesc.RefreshRate.Denominator = 1;
	fullScreenDesc.Windowed = !mIsFullScreen;



	IDXGIDevice* dxgiDevice = nullptr;
	ThrowIfFailed(mDirect3DDevice->QueryInterface(__uuidof(IDXGIDevice), 
		reinterpret_cast<void**>(&dxgiDevice)), "ID3D11Device :: QueryInterface() failed");

	IDXGIAdapter *dxgiAdapter = nullptr;
	HRESULT hr;
	if (FAILED(hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), 
		reinterpret_cast<void**>(&dxgiAdapter))))
	{
		ReleaseObject(dxgiDevice);
		throw GameException("IDXGIDevixce :: GetParent() failed retrieving adapter.", hr);
	}

	IDXGIFactory2* dxgiFactory = nullptr;
	if (FAILED(hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2),
		reinterpret_cast<void**>(&dxgiFactory))))
	{
		ReleaseObject(dxgiDevice);
		ReleaseObject(dxgiAdapter);
		throw GameException("IDXGIAdapter :: GetParent() failed retrieving factory.", hr);
	}

	
	if (FAILED(hr = dxgiFactory->CreateSwapChainForHwnd(dxgiDevice, mWindowHandle,
		&swapChainDesc, &fullScreenDesc, nullptr, &mSwapChain)))
	{
		ReleaseObject(dxgiDevice);
		ReleaseObject(dxgiAdapter);
		ReleaseObject(dxgiFactory);
		throw GameException("IDXGIDevice :: CreateSwapChainForHwnd(...) failed", hr);
	}
	ReleaseObject(dxgiDevice);
	ReleaseObject(dxgiAdapter);
	ReleaseObject(dxgiFactory);

	ID3D11Texture2D* backBuffer;
	if (FAILED(hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&backBuffer))))
	{
		throw GameException("mSwapChain::GetBuffer(...) failed", hr);
	}

	if (FAILED(hr = mDirect3DDevice->CreateRenderTargetView(backBuffer,
		nullptr, &mRenderTargetView))) 
	{
		ReleaseObject(backBuffer);
		throw GameException("IDGIDevice::CreateRenderTargetView(...) failed", hr);
	}
	ReleaseObject(backBuffer);

	//drawing 2D texture
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Width = mScreenWidth;
	depthStencilDesc.Height = mScreenHight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;

	if (mMultiSamplingEnabled)
	{	
		depthStencilDesc.SampleDesc.Count = mMultiSamplingCount;
		depthStencilDesc.SampleDesc.Quality = mMultiSamplingQualityLevels - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	//Creating 2D Texture and Depth stencil view

	ID3D11Texture2D* depthStencilBuffer;
	//ID3D11DepthStencilView* mDepthStencilView;

	if (FAILED(hr = mDirect3DDevice->CreateTexture2D(&depthStencilDesc,
		nullptr, &depthStencilBuffer)))
	{
		throw GameException("IDGIDevice::CreateTexture2D(...) failed", hr);
	}

	if (FAILED(hr = mDirect3DDevice->CreateDepthStencilView(depthStencilBuffer,
		nullptr, &mDepthStencilView)))
	{
		throw GameException("IDXGIDevice::CreateDepthStencilView(...) failed", hr);
	}

	//Setting Viewport
	D3D11_VIEWPORT mViewport;
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<float>(mScreenWidth);
	mViewport.Height = static_cast<float>(mScreenHight);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;


	//Associating the views to the ouput merger stage
	mDirect3DDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	mDirect3DDeviceContext->RSSetViewports(1, &mViewport);
}


void ShutDown(const wstring& className) {
	
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