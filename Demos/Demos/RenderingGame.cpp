#include "pch.h"

using namespace std;
using namespace DirectX;
using namespace Library;

namespace Rendering
{
	const XMVECTORF32 RenderingGame::BackgroundColor = Colors::CornflowerBlue;

	void RenderingGame::Initialize(UINT screenWidth, UINT screenHeight, HWND windowHandle)
	{


		mModelDemo = make_shared<ModelDemo>(*this);
		mComponents.push_back(mModelDemo);

		mKeyboard = make_shared<KeyboardComponent>(*this);
		mComponents.push_back(mKeyboard);

		mMouse = make_shared<MouseComponent>(*this);
		mComponents.push_back(mMouse);

		Game::Initialize(screenWidth, screenHeight, windowHandle);
	}

	void RenderingGame::Update(const Library::GameTime & gameTime)
	{
		if (mKeyboard->WasKeyPressedThisFrame(Keys::Escape)) 
		{
			Shutdown();
		}

		Game::Update(gameTime);
	}

	void RenderingGame::Draw(const Library::GameTime & gameTime)
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Game::Draw(gameTime);

		ThrowIfFailed(mSwapChain->Present(0, 0), "IDXGISwapChain::Present() failed.");
	}
}
