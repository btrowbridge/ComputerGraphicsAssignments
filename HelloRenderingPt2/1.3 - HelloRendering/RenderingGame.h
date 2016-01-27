#pragma once

#include <wrl.h>
//#include <DirectXMath.h>
#include "Game.h"
#include "PointDemo.h"

namespace Rendering
{
	class PointDemo;

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
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		std::unique_ptr<PointDemo> mPointDemo;

	};
}