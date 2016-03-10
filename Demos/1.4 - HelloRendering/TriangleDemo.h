#pragma once

#include <wrl.h>
#include <d3d11_2.h>
#include "DrawableGameComponent.h"

namespace Rendering
{
	class TriangleDemo : public Library::DrawableGameComponent
	{
	public:
		TriangleDemo(Library::Game& game);
		TriangleDemo(const TriangleDemo& rhs) = delete;
		TriangleDemo& operator=(const TriangleDemo& rhs) = delete;

		virtual void Initialize() override;
		virtual void Draw(const Library::GameTime& gameTime) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	};
}
