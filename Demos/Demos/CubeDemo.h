#pragma once

#include <wrl.h>
#include <d3d11_2.h>
#include <DirectXMath.h>
#include "DrawableGameComponent.h"
#include <memory>
#include "pch.h"


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
		virtual void Update(const Library::GameTime& gameTime) override;
	private:
		struct CBufferPerObject
		{
			DirectX::XMFLOAT4X4 WorldViewProjection;

			CBufferPerObject() : WorldViewProjection() {}
			CBufferPerObject(const DirectX::XMFLOAT4X4 wvp) :WorldViewProjection(wvp) {}
		};


		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBuffer;
		CBufferPerObject mCBufferPerObject;
		DirectX::XMFLOAT4X4 mWorldMatrix;
		
		//Game Component
		std::unique_ptr<Library::Camera> mCamera;
		std::unique_ptr<Library::KeyboardComponent> mKeyboard;
	};
}
