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
	class PointLight;
	class ProxyModel;
	class KeyboardComponent;
	class GamePadComponent;
}

namespace Rendering
{
	class PointLightingDemo : public Library::DrawableGameComponent
	{
	public:
		PointLightingDemo(Library::Game& game);
		PointLightingDemo(Library::Game& game, const std::shared_ptr<Library::Camera>& camera);
		PointLightingDemo(const PointLightingDemo& rhs) = delete;
		PointLightingDemo& operator=(const PointLightingDemo& rhs) = delete;

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

			VertexCBufferPerObject() : WorldViewProjection(), World() { }
			VertexCBufferPerObject(const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world) :
				WorldViewProjection(wvp), World(world) { }
		};
		struct VertexCBufferPerFrame
		{
			DirectX::XMFLOAT3 CameraPosition;
			float Padding;
			DirectX::XMFLOAT3 LightPosition;
			float LightRadius;

			VertexCBufferPerFrame() : CameraPosition(), LightPosition(), LightRadius() { }
			VertexCBufferPerFrame(const DirectX::XMFLOAT3 cp,
				const DirectX::XMFLOAT3 lp,
				float lr) :
				CameraPosition(cp), LightPosition(lp), LightRadius(lr) { }
		};

		struct PixelCBufferPerFrame
		{
			DirectX::XMFLOAT4 AmbientColor;
			DirectX::XMFLOAT4 LightColor;

			PixelCBufferPerFrame() :
				AmbientColor(DirectX::Colors::Black), LightColor(DirectX::Colors::White) { }
			PixelCBufferPerFrame(const DirectX::XMFLOAT4& ambientColor, const DirectX::XMFLOAT4& lightColor) :
				AmbientColor(ambientColor), LightColor(lightColor) { }
		};
		struct PixelCBufferPerObject {
			DirectX::XMFLOAT3 SpecularColor;
			float SpecularPower;

			PixelCBufferPerObject() : SpecularColor(DirectX::Colors::White), SpecularPower(25.0f) {};
			PixelCBufferPerObject(DirectX::XMFLOAT3 sc,
				float sp) : SpecularColor(sc), SpecularPower(sp) {};
		};

		void CreateVertexBuffer(const Library::Mesh& mesh, ID3D11Buffer** vertexBuffer) const;
		void ToggleAnimation();
		void UpdateAmbientLight(const Library::GameTime& gameTime);
		void UpdatePointLight(const Library::GameTime& gameTime, const DirectX::GamePad::State& gamePadState);
		void UpdateSpecularLight(const Library::GameTime & gameTime);

		void ToggleGamePadControls();

		static const float ModelRotationRate;
		const static float LightModulationRate;
		const static float LightMovementRate;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVSConstantBufferPO;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mPSConstantBufferPO;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVSConstantBufferPF;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mPSConstantBufferPF;


		DirectX::XMFLOAT4X4 mWorldMatrix;

		VertexCBufferPerObject mVSCBufferPerObject;
		VertexCBufferPerFrame mVSCBufferPerFrame;
		PixelCBufferPerFrame mPSCBufferPerFrame;
		PixelCBufferPerObject mPSCBufferPerObject;

		std::uint32_t mIndexCount;
		bool mAnimationEnabled;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mColorTexture;
		std::unique_ptr<Library::PointLight> mPointLight;
		std::unique_ptr<Library::ProxyModel> mProxyModel;
		Library::KeyboardComponent* mKeyboard;
		Library::GamePadComponent* mGamePad;
		bool mUseGamePadForPointLight;

		Library::RenderStateHelper mRenderStateHelper;
		std::unique_ptr<DirectX::SpriteBatch> mSpriteBatch;
		std::unique_ptr<DirectX::SpriteFont> mSpriteFont;
		DirectX::XMFLOAT2 mTextPosition;
	};
}
