#pragma once

#include <wrl.h>
#include <d3d11_2.h>
#include <DirectXMath.h>
#include "DrawableGameComponent.h"
#include <memory>
#include "pch.h"

using namespace DirectX;
using namespace Library;

namespace Rendering
{
	class ModelDemo : public Library::DrawableGameComponent
	{
	public:
		ModelDemo(Library::Game& game);
		ModelDemo(const ModelDemo& rhs) = delete;
		ModelDemo& operator=(const ModelDemo& rhs) = delete;

		virtual void CreateVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const;

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

		UINT mIndexCount;
		//Game Component
		std::unique_ptr<Library::Camera> mCamera;
		std::unique_ptr<Library::KeyboardComponent> mKeyboard;
	};
}
