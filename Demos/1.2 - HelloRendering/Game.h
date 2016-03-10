#pragma once

#include <Windows.h>
#include <wrl.h>
#include <string>
#include <sstream>

#include <d3d11_1.h>
#include <dxgi1_3.h>
#include <DirectXMath.h>

namespace Library
{
	class GameTime;

    class Game
    {
    public:
        Game();
		~Game() = default;
		Game(const Game& rhs) = delete;
		Game& operator=(const Game& rhs) = delete;

		void Initialize(UINT screenWidth, UINT screenHeight, HWND windowHandle);
		void Run();
		void Shutdown();

	private:
		void Update();
		void Draw();
    
        static const DirectX::XMVECTORF32 BackgroundColor;

		D3D_FEATURE_LEVEL mFeatureLevel;
		Microsoft::WRL::ComPtr<ID3D11Device1> mDirect3DDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> mDirect3DDeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> mSwapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthStencilView;
    };
}
