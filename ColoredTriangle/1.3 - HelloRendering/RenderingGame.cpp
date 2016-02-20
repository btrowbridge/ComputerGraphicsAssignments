#include "pch.h"
#include "PointDemo.h"
#include "TriangleDemo.h"

using namespace std;
using namespace DirectX;
using namespace Library;

namespace Rendering
{
	const XMVECTORF32 RenderingGame::BackgroundColor = Colors::CornflowerBlue;

	void RenderingGame::Initialize(UINT screenWidth, UINT screenHeight, HWND windowHandle)
	{
		Game::Initialize(screenWidth, screenHeight, windowHandle);

		mPointDemo = make_shared<PointDemo>(*this);
		mPointDemo->Initialize();

		mTriangleDemo = make_shared<TriangleDemo>(*this);
		mTriangleDemo->Initialize();
	}

	void RenderingGame::Update(const Library::GameTime & gameTime)
	{
		Game::Update(gameTime);
	}

	void RenderingGame::Draw(const Library::GameTime & gameTime)
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//mPointDemo->Draw(gameTime);
		mTriangleDemo->Draw(gameTime);

		Game::Draw(gameTime);

		ThrowIfFailed(mSwapChain->Present(0, 0), "IDXGISwapChain::Present() failed.");
	}
}