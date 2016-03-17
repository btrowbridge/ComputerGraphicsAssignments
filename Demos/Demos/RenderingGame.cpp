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


		mCamera->SetPosition(0.0f, 0.0f, 50.0f);
	}

	void RenderingGame::Update(const Library::GameTime & gameTime)
	{
		static float zPos = 50.0f;
		static float yPos = 0.0f;
		static float xPos = 0.0f;

		const float rateOfRotation = 1.0f;
		static float yRotation = 0.0f;
		static float xRotation = 0.0f;
		
		const float speed = 40.0f;

		if(mKeyboard->IsKeyDown(Keys::W)) {
			zPos -= speed * gameTime.ElapsedGameTimeSeconds().count();
		} else if (mKeyboard->IsKeyDown(Keys::S)) {
			zPos += speed * gameTime.ElapsedGameTimeSeconds().count();
			
		}

		if (mKeyboard->IsKeyDown(Keys::D)) {
			xPos += speed * gameTime.ElapsedGameTimeSeconds().count();
		}
		else if (mKeyboard->IsKeyDown(Keys::A)) {
			xPos -= speed * gameTime.ElapsedGameTimeSeconds().count();
		}

		if (mKeyboard->IsKeyDown(Keys::Q)) {
			yPos += speed * gameTime.ElapsedGameTimeSeconds().count();
		}
		else if (mKeyboard->IsKeyDown(Keys::E)) {
			yPos -= speed * gameTime.ElapsedGameTimeSeconds().count();
		}

		if (mKeyboard->IsKeyDown(Keys::Up)) {
			mCamera->ApplyRotation(XMMatrixRotationX(XMConvertToRadians(rateOfRotation)));

		}
		else if (mKeyboard->IsKeyDown(Keys::Down)) {
			mCamera->ApplyRotation(XMMatrixRotationX(XMConvertToRadians(-rateOfRotation)));
		}

		if (mKeyboard->IsKeyDown(Keys::Left)) {
			mCamera->ApplyRotation(XMMatrixRotationY(XMConvertToRadians(rateOfRotation)));

		}
		else if (mKeyboard->IsKeyDown(Keys::Right)) {
			mCamera->ApplyRotation(XMMatrixRotationY(XMConvertToRadians(-rateOfRotation)));
		}
		
		
		mCamera->SetPosition(xPos, yPos, zPos);
		
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
