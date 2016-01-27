#include "pch.h"
#include "RenderingGame.h"
#include <iostream>
using namespace std;
using namespace DirectX;
using namespace Library;
using namespace Microsoft::WRL;
namespace Rendering
{
	const XMVECTORF32 RenderingGame::BackgroundColor = Colors::CornflowerBlue;




	void RenderingGame::Initialize(UINT screenWidth, UINT screenHeight, HWND windowHandle)
	{
		Game::Initialize(screenWidth, screenHeight, windowHandle);
		
		mPointDemo = make_unique<PointDemo>(*this);
		mPointDemo.get()->Initialize();
		

		
		
	}

	void RenderingGame::Update(const Library::GameTime & gameTime)
	{
		Game::Update(gameTime);
	}

	void RenderingGame::Draw(const Library::GameTime & gameTime) 
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		
		mPointDemo->Draw(gameTime);
		
		UNREFERENCED_PARAMETER(gameTime);
		Game::Draw(gameTime);

		ThrowIfFailed(mSwapChain->Present(0, 0), "IDXGISwapChain::Present() failed.");
	}

}