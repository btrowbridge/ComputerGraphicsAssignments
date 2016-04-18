#include "pch.h"

using namespace std;
using namespace DirectX;
using namespace Library;

namespace Rendering
{
	const XMVECTORF32 RenderingGame::BackgroundColor = Colors::CornflowerBlue;

	void RenderingGame::Initialize(UINT screenWidth, UINT screenHeight, HWND windowHandle)
	{
		mCamera = make_shared<FirstPersonCamera>(*this);
		mComponents.push_back(mCamera);
		mServices.AddService(Camera::TypeIdClass(), mCamera.get());

		mKeyboard = make_shared<KeyboardComponent>(*this);
		mComponents.push_back(mKeyboard);
		mServices.AddService(KeyboardComponent::TypeIdClass(), mKeyboard.get());

		mMouse = make_shared<MouseComponent>(*this);
		mComponents.push_back(mMouse);
		mServices.AddService(MouseComponent::TypeIdClass(), mMouse.get());

		mGamePad = make_shared<GamePadComponent>(*this, 0);
		mComponents.push_back(mGamePad);
		mServices.AddService(GamePadComponent::TypeIdClass(), mGamePad.get());

		mGrid = make_shared<Grid>(*this, mCamera);
		mComponents.push_back(mGrid);

		//mDiffuseLightingDemo = make_shared<DiffuseLightingDemo>(*this, mCamera);
		//mComponents.push_back(mDiffuseLightingDemo);
		//demoQueue.push_back(mDiffuseLightingDemo);
		
		mPointLightingDemo = make_shared<PointLightingDemo>(*this, mCamera);
		mComponents.push_back(mPointLightingDemo);
		demoQueue.push_back(mPointLightingDemo);

		mSpecularLightingDemo = make_shared<SpecularLightingDemo>(*this, mCamera);
		mComponents.push_back(mSpecularLightingDemo);
		demoQueue.push_back(mSpecularLightingDemo);

		mSpotlightLightingDemo = make_shared<SpotlightLightingDemo>(*this, mCamera);
		mComponents.push_back(mSpotlightLightingDemo);
		demoQueue.push_back(mSpotlightLightingDemo);
		

		Game::Initialize(screenWidth, screenHeight, windowHandle);
		for (auto& component : demoQueue) {
			component->SetEnabled(false);
			component->SetVisible(false);
		}
		demoQueue.front()->SetEnabled(true);
		demoQueue.front()->SetVisible(true);

		SamplerStates::BorderColor = Colors::White;
		SamplerStates::Initialize(mDirect3DDevice.Get());
		RasterizerStates::Initialize(mDirect3DDevice.Get());
		BlendStates::Initialize(mDirect3DDevice.Get());

		mRenderStateHelper = make_shared<RenderStateHelper>(*this);
		mFpsComponent = make_shared<FpsComponent>(*this);
		mFpsComponent->Initialize();

		mCamera->SetPosition(0.0f, 5.0f, 20.0f);
	}

	void RenderingGame::Shutdown()
	{
		SamplerStates::Shutdown();
		RasterizerStates::Shutdown();
		BlendStates::Shutdown();
		Game::Shutdown();
	}

	void RenderingGame::Update(const Library::GameTime & gameTime)
	{
		mFpsComponent->Update(gameTime);

		auto& gamePadState = mGamePad->CurrentState();
		if (gamePadState.IsConnected())
		{
			if (mGamePad->WasButtonPressedThisFrame(GamePadButtons::RightStick))
			{
				mGrid->SetEnabled(!mGrid->Enabled());
				mGrid->SetVisible(!mGrid->Visible());
			}

			if (mGamePad->WasButtonPressedThisFrame(GamePadButtons::Back))
			{
				auto& temp = demoQueue.front();
				temp->SetEnabled(false);
				temp->SetVisible(false);
				demoQueue.pop_front();
				demoQueue.push_back(temp);
				demoQueue.front()->SetEnabled(true);
				demoQueue.front()->SetVisible(true);
			}
		}

		if (mKeyboard->WasKeyPressedThisFrame(Keys::Escape))
		{
			Exit();
		}

		Game::Update(gameTime);
	}

	void RenderingGame::Draw(const Library::GameTime & gameTime)
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Game::Draw(gameTime);

		mRenderStateHelper->SaveAll();
		mFpsComponent->Draw(gameTime);
		mRenderStateHelper->RestoreAll();

		ThrowIfFailed(mSwapChain->Present(1, 0), "IDXGISwapChain::Present() failed.");
	}

	void RenderingGame::Exit()
	{
		PostQuitMessage(0);
	}
}