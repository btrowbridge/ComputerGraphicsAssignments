#pragma once
#include "pch.h"

namespace GameMaker {
	class Game {
		
		const XMVECTORF32 backgroundColor = { 0.0f, 0.750f, 1.0f, 1.0f }; //RGBA
		
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


		
	};
}