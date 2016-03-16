#pragma once

#include <wrl.h>
#include <memory>
#include <DirectXMath.h>
#include "Game.h"

namespace Library
{
	class KeyboardComponent;
	class GamePadComponent;
	class MouseComponent;
	class FpsComponent;
	class RenderStateHelper;
}

namespace Rendering
{
	class ModelDemo;

	class RenderingGame : public Library::Game
	{
	public:
		RenderingGame() = default;

		virtual void Initialize(UINT screenWidth, UINT screenHeight, HWND windowHandle) override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;
		
	private:
		static const DirectX::XMVECTORF32 BackgroundColor;

		std::shared_ptr<Library::Camera> mCamera;
		std::shared_ptr<Library::KeyboardComponent> mKeyboard;		
		std::shared_ptr<Library::MouseComponent> mMouse;
		std::shared_ptr<Library::GamePadComponent> mGamePad;
		std::shared_ptr<Library::FpsComponent> mFpsComponent;
		std::shared_ptr<Library::RenderStateHelper> mRenderStateHelper;
		std::shared_ptr<ModelDemo> mModelDemo;
	};
}