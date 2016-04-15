#pragma once

#include <wrl.h>
#include <d3d11_2.h>
#include "DrawableGameComponent.h"

namespace Rendering
{
	class PointDemo : public Library::DrawableGameComponent
	{
	public:
		PointDemo(Library::Game& game);
		PointDemo(const PointDemo& rhs) = delete;
		PointDemo& operator=(const PointDemo& rhs) = delete;

		virtual void Initialize() override;
		virtual void Draw(const Library::GameTime& gameTime) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	};
}
