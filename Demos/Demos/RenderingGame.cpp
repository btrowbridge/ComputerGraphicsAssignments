#include "pch.h"

using namespace std;
using namespace DirectX;
using namespace Library;

namespace Rendering
{
	const XMVECTORF32 RenderingGame::BackgroundColor = Colors::CornflowerBlue;

	void RenderingGame::Initialize(UINT screenWidth, UINT screenHeight, HWND windowHandle)
	{
		mCamera = make_shared<PerspectiveCamera>(*this);
		mComponents.push_back(mCamera);
		mServices.AddService(PerspectiveCamera::TypeIdClass(), mCamera.get());

		mKeyboard = make_shared<KeyboardComponent>(*this);
		mComponents.push_back(mKeyboard);
		mServices.AddService(KeyboardComponent::TypeIdClass(), mKeyboard.get());

		mMouse = make_shared<MouseComponent>(*this);
		mComponents.push_back(mMouse);
		mServices.AddService(MouseComponent::TypeIdClass(), mMouse.get());

		mGamePad = make_shared<GamePadComponent>(*this, 0);
		mComponents.push_back(mGamePad);
		mServices.AddService(GamePadComponent::TypeIdClass(), mGamePad.get());

		mModelDemo = make_shared<ModelDemo>(*this, mCamera);
		mModelDemo->SetEnabled(true);
		mModelDemo->SetVisible(true);
		mComponents.push_back(mModelDemo);
		
		Game::Initialize(screenWidth, screenHeight, windowHandle);

		mRenderStateHelper = make_shared<RenderStateHelper>(*this);


		mCamera->SetPosition(0.0f, 0.0f, 20.0f);
	}

	void RenderingGame::Update(const Library::GameTime & gameTime)
	{

		mCamera->Update(gameTime);
		if (mKeyboard->WasKeyPressedThisFrame(Keys::Escape))
		{
			PostQuitMessage(0);
		}

		if (mKeyboard->WasKeyPressedThisFrame(Keys::Space))
		{
			mModelDemo->SetAnimationEnabled(!mModelDemo->AnimationEnabled());
		}

		Game::Update(gameTime);
	}

	void RenderingGame::Draw(const Library::GameTime & gameTime)
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Game::Draw(gameTime);
		
		mRenderStateHelper->SaveAll();

		mRenderStateHelper->RestoreAll();

		ThrowIfFailed(mSwapChain->Present(1, 0), "IDXGISwapChain::Present() failed.");
	}
}
