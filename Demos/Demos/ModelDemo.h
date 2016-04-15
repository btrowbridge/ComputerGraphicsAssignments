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
		ModelDemo(Game & game, const std::shared_ptr<Library::Camera>& camera);
		ModelDemo(const ModelDemo& rhs) = delete;
		ModelDemo& operator=(const ModelDemo& rhs) = delete;

		void SetAnimationEnabled(bool isEnabled);
		bool AnimationEnabled();

		virtual void CreateVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const;

		virtual void Initialize() override;
		void SetDirectionLight(float x, float y, float z);
		void SetAmbientColor(float r, float b, float g, float a);
		virtual void Draw(const Library::GameTime& gameTime) override;
		virtual void Update(const Library::GameTime& gameTime) override;
	private:
		struct CBufferPerObjectVS
		{
			DirectX::XMFLOAT4X4 WorldViewProjection;
			DirectX::XMFLOAT4X4 World;

			CBufferPerObjectVS() : WorldViewProjection(), World() {}
			CBufferPerObjectVS(const DirectX::XMFLOAT4X4 wvp, const DirectX::XMFLOAT4X4 w) :WorldViewProjection(wvp), World(w) {}
		};
		struct CBufferPerFrameVS
		{
			DirectX::XMFLOAT3 CameraPosition;
			float Padding;
			DirectX::XMFLOAT3 LightPosition;
			float LightRadius;

			CBufferPerFrameVS() : CameraPosition(), LightPosition(), Padding(), LightRadius(){}
			CBufferPerFrameVS(const DirectX::XMFLOAT3 cd, const DirectX::XMFLOAT3 lp, const float lr) : CameraPosition(cd), LightPosition(lp), Padding(), LightRadius(lr) {}
		};

		struct CBufferPerObjectPS
		{
			DirectX::XMFLOAT3 SpecularColor;
			float SpecularPower;

			CBufferPerObjectPS() : SpecularColor(), SpecularPower() {}
			CBufferPerObjectPS(const DirectX::XMFLOAT3 sc, const float sp) : SpecularColor(sc), SpecularPower(sp) {}
		};
		struct CBufferPerFramePS
		{
			DirectX::XMFLOAT4 DirectionLight;
			DirectX::XMFLOAT4 AmbientColor;
			DirectX::XMFLOAT3 LightColor;
			float Padding;

			CBufferPerFramePS() : DirectionLight(), AmbientColor(), LightColor(), Padding() {}
			CBufferPerFramePS(const DirectX::XMFLOAT4 dl, const DirectX::XMFLOAT4 ac, const DirectX::XMFLOAT3 lc) : DirectionLight(dl), AmbientColor(ac), LightColor(lc), Padding() {}
		};

		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBufferPerFrameVS;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBufferPerFramePS;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBufferPerObjectVS;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBufferPerObjectPS;

		CBufferPerFrameVS mCBufferPerFrameVS;
		CBufferPerObjectVS mCBufferPerObjectVS;
		CBufferPerFramePS mCBufferPerFramePS;
		CBufferPerObjectPS mCBufferPerObjectPS;

		DirectX::XMFLOAT3 mLightDirection;
		DirectX::XMFLOAT3 mAmbientColor;
		DirectX::XMFLOAT4X4 mWorldMatrix;

		UINT mIndexCount;
		bool mAnimationEnabled;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mColorTexture;
	};
}
