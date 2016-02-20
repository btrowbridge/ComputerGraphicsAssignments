#pragma once

#include <wrl.h>
#include <d3d11_2.h>
#include <DirectXMath.h>
#include "DrawableGameComponent.h"

namespace Rendering
{
	class CubeDemo : public Library::DrawableGameComponent
	{
	public:
		CubeDemo(Library::Game& game);
		CubeDemo(const CubeDemo& rhs) = delete;
		CubeDemo& operator=(const CubeDemo& rhs) = delete;

		virtual void Initialize() override;
		virtual void Draw(const Library::GameTime& gameTime) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		DirectX::XMFLOAT4X4 mWorldMatrix;
	};
}
