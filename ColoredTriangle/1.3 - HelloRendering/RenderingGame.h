#pragma once

#include <wrl.h>
#include <DirectXMath.h>
#include "Game.h"
#include <d3d11_2.h>
#include <memory>

namespace Rendering
{
	class PointDemo;
	class TriangleDemo;

	class RenderingGame : public Library::Game
	{
	public:
		RenderingGame() = default;
		RenderingGame(const RenderingGame& rhs) = delete;
		RenderingGame& operator=(const RenderingGame& rhs) = delete;

		virtual void Initialize(UINT screenWidth, UINT screenHeight, HWND windowHandle) override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;

	private:
		static const DirectX::XMVECTORF32 BackgroundColor;
		std::shared_ptr<PointDemo> mPointDemo;
		std::shared_ptr<TriangleDemo> mTriangleDemo;
	};
}