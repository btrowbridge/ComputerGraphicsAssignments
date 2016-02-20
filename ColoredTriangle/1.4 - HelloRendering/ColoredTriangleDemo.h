#pragma once

#include <wrl.h>
#include <d3d11_2.h>
#include "DrawableGameComponent.h"

namespace Rendering
{
	class ColoredTriangleDemo : public Library::DrawableGameComponent
	{
	public:
		ColoredTriangleDemo(Library::Game& game);
		ColoredTriangleDemo(const ColoredTriangleDemo& rhs) = delete;
		ColoredTriangleDemo& operator=(const ColoredTriangleDemo& rhs) = delete;

		virtual void Initialize() override;
		virtual void Draw(const Library::GameTime& gameTime) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	};
}
