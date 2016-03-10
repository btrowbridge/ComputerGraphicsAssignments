#pragma once

#include <wrl.h>
#include <memory>
#include <DirectXMath.h>
#include "Game.h"

namespace Library 
{
	class KeyboardComponent;
	class MouseComponent;
}

namespace Rendering
{
	class PointDemo;
	class TriangleDemo;
	class ColoredTriangleDemo;
	class CubeDemo;
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

		std::shared_ptr<ColoredTriangleDemo> mColoredTriangleDemo;
		std::shared_ptr<CubeDemo> mCubeDemo;
		std::shared_ptr<ModelDemo> mModelDemo;

		std::shared_ptr<Library::KeyboardComponent> mKeyboard;
		std::shared_ptr<Library::MouseComponent> mMouse;
		
	};
}