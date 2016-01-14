#pragma once
#include "pch.h"
namespace GameLibrary {
	
	class Game{
	public:
		/*Previously presented members removed for brevity*/

		ID3D11Device1* Direct3DDevice() const;
		ID3D11DeviceContext1* Direct3DDeviceContext() const;
		bool DepthBufferEnabled() const;
		bool IsFullScreen() const;
		const D3D11_TEXTURE2D_DESC & BackBufferDesc() const;
		const D3D11_VIEWPORT& Viewport() const;

	protected:

		/*Previously presented members removed for brevity*/
		virtual void InitializeDirectX();

		static const UINT DefaultFrameRate;
		static const UINT DefaultMultiSamplingCount;
		
		D3D_FEATURE_LEVEL mFeatureLevel;
		ID3D11Device1* mDirect3DDevice; 
		ID3D11DeviceContext1* mDirect3DDeviceContext;
		IDXGISwapChain1* mSwapChain;

		UINT mFrameRate;
		bool mIsFullScreen;
		bool mDepthStencilBufferEnabled;
		bool mMultiSamplingEnabled;
		UINT mMultiSamplingCount;
		UINT mMultiSamplingQualityLevels;

		ID3D11Texture2D* mDepthStencilBuffer;
		D3D11_TEXTURE2D_DESC mBackBufferDesc;
		ID3D11RenderTargetView* mRenderTargetView;
		ID3D11DepthStencilView* mDepthStencilView;
		D3D11_VIEWPORT mViewport;

		
	};
}