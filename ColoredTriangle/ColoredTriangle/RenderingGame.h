#pragma once

#include <wrl.h>
#include <memory>
#include <DirectXMath.h>
#include "Game.h"

namespace Rendering
{
	class PointDemo;
	class TriangleDemo;
	class ColoredTriangleDemo;
	class CubeDemo;

	class RenderingGame : public Library::Game
	{
	public:
		RenderingGame() = default;

		virtual void Initialize(UINT screenWidth, UINT screenHeight, HWND windowHandle) override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;
		
	private:
		static const DirectX::XMVECTORF32 BackgroundColor;

		std::shared_ptr<PointDemo> mPointDemo;
		std::shared_ptr<TriangleDemo> mTriangleDemo;
		std::shared_ptr<ColoredTriangleDemo> mColoredTriangleDemo;
		std::shared_ptr<CubeDemo> mCubeDemo;
	};
}