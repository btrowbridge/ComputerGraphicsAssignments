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
	class SpotLight;
	class ProxyModel;
	class KeyboardComponent;
	class GamePadComponent;
}

namespace Rendering
{
	class SpotlightLightingDemo : public Library::DrawableGameComponent
	{
	public:
		SpotlightLightingDemo(Library::Game& game);
		SpotlightLightingDemo(Library::Game& game, const std::shared_ptr<Library::Camera>& camera);
		SpotlightLightingDemo(const SpotlightLightingDemo& rhs) = delete;
		SpotlightLightingDemo& operator=(const SpotlightLightingDemo& rhs) = delete;

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
			DirectX::XMFLOAT3 LightPosition;
			float LightRadius;
			DirectX::XMFLOAT3 LightLookAt;
			float Padding;

			VertexCBufferPerFrame()
				: LightPosition(0.0f, 0.0f, 0.0f), LightRadius(10.0f), LightLookAt(0.0f, 0.0f, -1.0f)
			{
			}

			VertexCBufferPerFrame(const DirectX::XMFLOAT3& lightPosition, float lightRadius, const DirectX::XMFLOAT3& lightLookAt)
				: LightPosition(lightPosition), LightRadius(lightRadius), LightLookAt(lightLookAt)
			{
			}
		};

		struct PixelCBufferPerFrame
		{
			DirectX::XMFLOAT4 AmbientColor;
			DirectX::XMFLOAT4 LightColor;
			DirectX::XMFLOAT3 LightPosition;
			float SpotLightInnerAngle;
			float SpotLightOuterAngle;
			DirectX::XMFLOAT3 CameraPosition;

			PixelCBufferPerFrame()
				: AmbientColor(0.0f, 0.0f, 0.0f, 0.0f), LightColor(1.0f, 1.0f, 1.0f, 1.0f), LightPosition(0.0f, 0.0f, 0.0f),
				SpotLightInnerAngle(0.75f), SpotLightOuterAngle(0.25f), CameraPosition(0.0f, 0.0f, 0.0f)
			{
			}

			PixelCBufferPerFrame(const DirectX::XMFLOAT4& ambientColor, const DirectX::XMFLOAT4& lightColor, const DirectX::XMFLOAT3& lightPosition, float spotLightInnerAngle, float spotLightOuterAngle, const DirectX::XMFLOAT3& cameraPosition)
				: AmbientColor(ambientColor), LightColor(lightColor), LightPosition(lightPosition),
				SpotLightInnerAngle(spotLightInnerAngle), SpotLightOuterAngle(spotLightOuterAngle), CameraPosition(cameraPosition)
			{
			}
		};
		struct PixelCBufferPerObject {
			DirectX::XMFLOAT3 SpecularColor;
			float SpecularPower;

			PixelCBufferPerObject() : SpecularColor(1.0f, 1.0f, 1.0f), SpecularPower(25.0f) { }

			PixelCBufferPerObject(const DirectX::XMFLOAT3& specularColor, float specularPower)
				: SpecularColor(specularColor), SpecularPower(specularPower)
			{
			}
		};

		void CreateVertexBuffer(const Library::Mesh& mesh, ID3D11Buffer** vertexBuffer) const;
		void ToggleAnimation();
		void UpdateAmbientLight(const Library::GameTime& gameTime);
		void UpdateSpotLight(const Library::GameTime& gameTime, const DirectX::GamePad::State& gamePadState);
		void UpdateSpecularLight(const Library::GameTime& gameTime);
		void ToggleGamePadControls();

		static const float ModelRotationRate;
		static const DirectX::XMFLOAT2 LightRotationRate;
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
		std::unique_ptr<Library::SpotLight> mSpotLight;
		std::unique_ptr<Library::ProxyModel> mProxyModel;
		Library::KeyboardComponent* mKeyboard;
		Library::GamePadComponent* mGamePad;
		bool mUseGamePadForSpotLight;
		bool mIsAStarWars;

		Library::RenderStateHelper mRenderStateHelper;
		std::unique_ptr<DirectX::SpriteBatch> mSpriteBatch;
		std::unique_ptr<DirectX::SpriteFont> mSpriteFont;
		DirectX::XMFLOAT2 mTextPosition;
	};
}
