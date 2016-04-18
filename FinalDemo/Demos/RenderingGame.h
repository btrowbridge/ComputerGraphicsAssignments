#pragma once

#include <wrl.h>
#include <memory>
#include <DirectXMath.h>
#include "Game.h"
#include <queue>

namespace Library
{
	class KeyboardComponent;
	class GamePadComponent;
	class MouseComponent;
	class FpsComponent;
	class RenderStateHelper;
	class Grid;
	class DrawableGameComponent;
}

namespace Rendering
{
	class DiffuseLightingDemo;
	class PointLightingDemo;
	class SpecularLightingDemo;
	class SpotlightLightingDemo;

	class RenderingGame : public Library::Game
	{
	public:
		RenderingGame() = default;

		virtual void Initialize(UINT screenWidth, UINT screenHeight, HWND windowHandle) override;
		virtual void Shutdown() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;

	private:
		void Exit();

		static const DirectX::XMVECTORF32 BackgroundColor;

		std::shared_ptr<Library::Camera> mCamera;
		std::shared_ptr<Library::KeyboardComponent> mKeyboard;
		std::shared_ptr<Library::MouseComponent> mMouse;
		std::shared_ptr<Library::GamePadComponent> mGamePad;
		std::shared_ptr<Library::FpsComponent> mFpsComponent;
		std::shared_ptr<Library::RenderStateHelper> mRenderStateHelper;
		std::shared_ptr<Library::Grid> mGrid;
		std::shared_ptr<DiffuseLightingDemo> mDiffuseLightingDemo;
		std::shared_ptr<PointLightingDemo> mPointLightingDemo;
		std::shared_ptr<SpecularLightingDemo> mSpecularLightingDemo;
		std::shared_ptr<SpotlightLightingDemo> mSpotlightLightingDemo;

		std::deque<std::shared_ptr<Library::DrawableGameComponent>> demoQueue;
	};
}