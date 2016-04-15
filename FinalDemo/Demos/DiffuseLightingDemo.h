#pragma once

#include <wrl.h>
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <cstdint>
#include <memory>
#include "DrawableGameComponent.h"

namespace Library
{
	class Camera;
	class Mesh;
	class DirectionalLight;
	class ProxyModel;
	class KeyboardComponent;
	class GamePadComponent;
}

namespace Rendering
{
	class DiffuseLightingDemo : public Library::DrawableGameComponent
	{
	public:
		DiffuseLightingDemo(Library::Game& game);
		DiffuseLightingDemo(Library::Game& game, const std::shared_ptr<Library::Camera>& camera);
		DiffuseLightingDemo(const DiffuseLightingDemo& rhs) = delete;
		DiffuseLightingDemo& operator=(const DiffuseLightingDemo& rhs) = delete;

		bool AnimationEnabled() const;
		void SetAnimationEnabled(bool enabled);

		virtual void Initialize() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;

	private:
		struct VertexCBufferPerObject
		{
			DirectX::XMFLOAT4X4 WorldViewProjection;
			DirectX::XMFLOAT4X4 World;

			VertexCBufferPerObject() { }
			VertexCBufferPerObject(const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world) :
				WorldViewProjection(wvp), World(world) { }
		};

		struct PixelCBufferPerFrame
		{
			DirectX::XMFLOAT4 AmbientColor;
			DirectX::XMFLOAT3 LightDirection;
			float Padding2;
			DirectX::XMFLOAT4 LightColor;

			PixelCBufferPerFrame() :
				AmbientColor(DirectX::Colors::Black), LightDirection(0.0f, 0.0f, 1.0f), LightColor(DirectX::Colors::White) { }
			PixelCBufferPerFrame(const DirectX::XMFLOAT4& ambientColor, const DirectX::XMFLOAT3& lightDirection, const DirectX::XMFLOAT4& lightColor) :
				AmbientColor(ambientColor), LightDirection(lightDirection), LightColor(lightColor) { }
		};

		void CreateVertexBuffer(const Library::Mesh& mesh, ID3D11Buffer** vertexBuffer) const;
		void ToggleAnimation();
		void UpdateAmbientLight(const Library::GameTime& gameTime);
		void UpdateDirectionalLight(const Library::GameTime& gameTime, const DirectX::GamePad::State& gamePadState);
		void ToggleGamePadControls();

		static const float ModelRotationRate;
		static const DirectX::XMFLOAT2 LightRotationRate;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVSConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mPSConstantBuffer;
		DirectX::XMFLOAT4X4 mWorldMatrix;
		VertexCBufferPerObject mVSCBufferPerObject;
		PixelCBufferPerFrame mPSCBufferPerFrame;
		std::uint32_t mIndexCount;
		bool mAnimationEnabled;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mColorTexture;
		std::unique_ptr<Library::DirectionalLight> mDirectionalLight;
		std::unique_ptr<Library::ProxyModel> mProxyModel;
		Library::KeyboardComponent* mKeyboard;
		Library::GamePadComponent* mGamePad;
		bool mUseGamePadForDirectionalLight;
	};
}
