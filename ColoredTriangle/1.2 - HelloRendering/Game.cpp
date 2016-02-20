#include "pch.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

namespace Library
{
    const XMVECTORF32 Game::BackgroundColor = Colors::CornflowerBlue;

    Game::Game()
    {
    }

	void Game::Initialize(UINT screenWidth, UINT screenHeight, HWND windowHandle)
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

		ComPtr<ID3D11Device> direct3DDevice;
		ComPtr<ID3D11DeviceContext> direct3DDeviceContext;
		ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, direct3DDevice.ReleaseAndGetAddressOf(), &mFeatureLevel, direct3DDeviceContext.ReleaseAndGetAddressOf()), "D3D11CreateDevice() failed");
		ThrowIfFailed(direct3DDevice.As(&mDirect3DDevice));
		ThrowIfFailed(direct3DDeviceContext.As(&mDirect3DDeviceContext));

		UINT multiSamplingCount = 4;
		UINT multiSamplingQualityLevels;
		ThrowIfFailed(mDirect3DDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, multiSamplingCount, &multiSamplingQualityLevels), "CheckMultisampleQualityLevels() failed.");
		if (multiSamplingQualityLevels == 0)
		{
			throw GameException("Unsupported multi-sampling quality");
		}

#ifndef NDEBUG
		ComPtr<ID3D11Debug> d3dDebug;
		HRESULT hr = mDirect3DDevice.As(&d3dDebug);
		if (SUCCEEDED(hr))
		{
			ComPtr<ID3D11InfoQueue> d3dInfoQueue;
			hr = d3dDebug.As(&d3dInfoQueue);
			if (SUCCEEDED(hr))
			{
#ifdef _DEBUG
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
				D3D11_MESSAGE_ID hide[] =
				{
					D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS
				};
				D3D11_INFO_QUEUE_FILTER filter = { 0 };
				filter.DenyList.NumIDs = _countof(hide);
				filter.DenyList.pIDList = hide;
				d3dInfoQueue->AddStorageFilterEntries(&filter);
			}
		}
#endif

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.Width = screenWidth;
		swapChainDesc.Height = screenHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.SampleDesc.Count = multiSamplingCount;
		swapChainDesc.SampleDesc.Quality = multiSamplingQualityLevels - 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		ComPtr<IDXGIDevice3> dxgiDevice;
		ThrowIfFailed(mDirect3DDevice.As(&dxgiDevice));

		ComPtr<IDXGIAdapter> dxgiAdapter;
		ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

		ComPtr<IDXGIFactory2> dxgiFactory;
		ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = { 0 };
		fullScreenDesc.RefreshRate.Numerator = 60;
		fullScreenDesc.RefreshRate.Denominator = 1;
		fullScreenDesc.Windowed = true;
		ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(mDirect3DDevice.Get(), windowHandle, &swapChainDesc, &fullScreenDesc, nullptr, mSwapChain.ReleaseAndGetAddressOf()), "IDXGIDevice::CreateSwapChainForHwnd() failed.");

		ComPtr<ID3D11Texture2D> backBuffer;
		ThrowIfFailed(mSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)), "IDXGISwapChain1::GetBuffer() failed.");
		ThrowIfFailed(mDirect3DDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, mRenderTargetView.GetAddressOf()), "IDXGIDevice::CreateRenderTargetView() failed.");

		D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
		depthStencilDesc.Width = screenWidth;
		depthStencilDesc.Height = screenHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.SampleDesc.Count = multiSamplingCount;
		depthStencilDesc.SampleDesc.Quality = multiSamplingQualityLevels - 1;

		ComPtr<ID3D11Texture2D> depthStencilBuffer;
		ThrowIfFailed(mDirect3DDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencilBuffer.GetAddressOf()), "IDXGIDevice::CreateTexture2D() failed.");
		ThrowIfFailed(mDirect3DDevice->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, mDepthStencilView.ReleaseAndGetAddressOf()), "IDXGIDevice::CreateDepthStencilView() failed.");

		D3D11_VIEWPORT viewport = CD3D11_VIEWPORT(0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight));
		mDirect3DDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
		mDirect3DDeviceContext->RSSetViewports(1, &viewport);
	}

	void Game::Run()
	{
		Update();
		Draw();
	}

	void Game::Shutdown()
	{
		mDirect3DDeviceContext->ClearState();
		mDirect3DDeviceContext->Flush();

		mDepthStencilView = nullptr;
		mRenderTargetView = nullptr;
		mSwapChain = nullptr;
		mDirect3DDeviceContext = nullptr;
		mDirect3DDevice = nullptr;
	}

	void Game::Update()
	{
		//TODO: Game-specific code
	}

    void Game::Draw()
    {
        mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&BackgroundColor));
        mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//TODO: Game-specific code
		
		ThrowIfFailed(mSwapChain->Present(0, 0), "IDXGISwapChain::Present() failed.");
    }
}