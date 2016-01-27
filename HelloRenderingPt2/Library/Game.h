#pragma once

#include <Windows.h>
#include <wrl.h>
#include <string>
#include <sstream>

#include <d3d11_2.h>
#include <dxgi1_3.h>
#include <DirectXMath.h>

#include "GameClock.h"
#include "GameTime.h"

namespace Library
{
    class Game
    {
    public:
        Game() = default;
		virtual ~Game() = default;

		Game(const Game& rhs) = delete;
		Game& operator=(const Game& rhs) = delete;

		ID3D11Device2* Direct3DDevice() const;
		ID3D11DeviceContext2* Direct3DDeviceContext() const;
		IDXGISwapChain1* SwapChain() const;
		ID3D11RenderTargetView* RenderTargetView() const;
		ID3D11DepthStencilView* DepthStencilView() const;		
		const D3D11_VIEWPORT& Viewport() const;

		virtual void Initialize(UINT screenWidth, UINT screenHeight, HWND windowHandle);
		virtual void Run();
		virtual void Shutdown();

	protected:
		virtual void Update(const GameTime& gameTime);
		virtual void Draw(const GameTime& gameTime);
				
		Microsoft::WRL::ComPtr<ID3D11Device2> mDirect3DDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext2> mDirect3DDeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> mSwapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthStencilView;
		D3D11_VIEWPORT mViewport;
		D3D_FEATURE_LEVEL mFeatureLevel;

		GameClock mGameClock;
		GameTime mGameTime;
    };
}
