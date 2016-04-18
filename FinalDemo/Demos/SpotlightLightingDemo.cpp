#include "pch.h"

using namespace std;
using namespace Library;
using namespace DirectX;

namespace Rendering
{
	const float SpotlightLightingDemo::ModelRotationRate = XM_PI;
	const XMFLOAT2 SpotlightLightingDemo::LightRotationRate = XMFLOAT2(XMConvertToRadians(500.0f), XMConvertToRadians(500.0f));
	const float SpotlightLightingDemo::LightModulationRate = UCHAR_MAX;
	const float SpotlightLightingDemo::LightMovementRate = 10.0f;

	SpotlightLightingDemo::SpotlightLightingDemo(Game& game) :
		DrawableGameComponent(game), mWorldMatrix(MatrixHelper::Identity), mIndexCount(0),
		mAnimationEnabled(false), mUseGamePadForSpotLight(false), mRenderStateHelper(game), mSpriteBatch(nullptr), mSpriteFont(nullptr), mTextPosition(0.0f, 40.0f)
	{
	}

	SpotlightLightingDemo::SpotlightLightingDemo(Game & game, const shared_ptr<Camera>& camera) :
		DrawableGameComponent(game, camera), mWorldMatrix(MatrixHelper::Identity), mIndexCount(0),
		mAnimationEnabled(false), mUseGamePadForSpotLight(false), mRenderStateHelper(game), mSpriteBatch(nullptr), mSpriteFont(nullptr), mTextPosition(0.0f, 40.0f)
	{
	}

	bool SpotlightLightingDemo::AnimationEnabled() const
	{
		return mAnimationEnabled;
	}

	void SpotlightLightingDemo::SetAnimationEnabled(bool enabled)
	{
		mAnimationEnabled = enabled;
	}

	void SpotlightLightingDemo::Initialize()
	{
		// Load a compiled vertex shader
		vector<char> compiledVertexShader;
		Utility::LoadBinaryFile(L"Content\\Shaders\\SpotlightLightingDemoVS.cso", compiledVertexShader);
		ThrowIfFailed(mGame->Direct3DDevice()->CreateVertexShader(&compiledVertexShader[0], compiledVertexShader.size(), nullptr, mVertexShader.ReleaseAndGetAddressOf()), "ID3D11Device::CreatedVertexShader() failed.");

		// Load a compiled pixel shader
		vector<char> compiledPixelShader;
		Utility::LoadBinaryFile(L"Content\\Shaders\\SpotlightLightingDemoPS.cso", compiledPixelShader);
		ThrowIfFailed(mGame->Direct3DDevice()->CreatePixelShader(&compiledPixelShader[0], compiledPixelShader.size(), nullptr, mPixelShader.ReleaseAndGetAddressOf()), "ID3D11Device::CreatedPixelShader() failed.");

		// Create an input layout
		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ThrowIfFailed(mGame->Direct3DDevice()->CreateInputLayout(inputElementDescriptions, ARRAYSIZE(inputElementDescriptions), &compiledVertexShader[0], compiledVertexShader.size(), mInputLayout.ReleaseAndGetAddressOf()), "ID3D11Device::CreateInputLayout() failed.");

		// Load the model
		unique_ptr<Library::Model> model = make_unique<Library::Model>("Content\\Models\\Sphere.obj.bin");

		// Create vertex and index buffers for the model
		Mesh* mesh = model->Meshes().at(0).get();
		CreateVertexBuffer(*mesh, mVertexBuffer.ReleaseAndGetAddressOf());
		mesh->CreateIndexBuffer(*mGame->Direct3DDevice(), mIndexBuffer.ReleaseAndGetAddressOf());
		mIndexCount = static_cast<UINT>(mesh->Indices().size());

		D3D11_BUFFER_DESC constantBufferDesc = { 0 };
		constantBufferDesc.ByteWidth = sizeof(VertexCBufferPerObject);
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ThrowIfFailed(mGame->Direct3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, mVSConstantBufferPO.ReleaseAndGetAddressOf()), "ID3D11Device::CreateBuffer() failed.");

		constantBufferDesc.ByteWidth = sizeof(VertexCBufferPerFrame);
		ThrowIfFailed(mGame->Direct3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, mVSConstantBufferPF.ReleaseAndGetAddressOf()), "ID3D11Device::CreateBuffer() failed.");

		constantBufferDesc.ByteWidth = sizeof(PixelCBufferPerFrame);
		ThrowIfFailed(mGame->Direct3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, mPSConstantBufferPF.ReleaseAndGetAddressOf()), "ID3D11Device::CreateBuffer() failed.");

		constantBufferDesc.ByteWidth = sizeof(PixelCBufferPerObject);
		ThrowIfFailed(mGame->Direct3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, mPSConstantBufferPO.ReleaseAndGetAddressOf()), "ID3D11Device::CreateBuffer() failed.");

		// Load a texture
		wstring textureName = L"Content\\Textures\\Earthatday.dds";
		ThrowIfFailed(CreateDDSTextureFromFile(mGame->Direct3DDevice(), textureName.c_str(), nullptr, mColorTexture.ReleaseAndGetAddressOf()), "CreateDDSTextureFromFile() failed.");

		mSpriteBatch = std::make_unique<SpriteBatch>(mGame->Direct3DDeviceContext());
		mSpriteFont = std::make_unique<SpriteFont>(mGame->Direct3DDevice(), L"Content\\Fonts\\Arial_14_Regular.spritefont");

		
		//Create Proxy and Spotlight
		mProxyModel = std::make_unique<ProxyModel>(*mGame, mCamera, "Content\\Models\\darkfighter6.obj.bin", 0.05f);
		mProxyModel->Initialize();
		mProxyModel->ApplyRotation(XMMatrixRotationY(-XM_PIDIV2));

		mSpotLight = std::make_unique<SpotLight>(*mGame);
		mSpotLight->SetRadius(50.0f);
		mSpotLight->SetPosition(0.0f, 0.0f, 10.0f);
		mProxyModel->SetPosition(mSpotLight->Position());
		mVSCBufferPerFrame.LightPosition = mSpotLight->Position();
		mVSCBufferPerFrame.LightRadius = mSpotLight->Radius();
		mVSCBufferPerFrame.LightLookAt = mSpotLight->Direction();

		mPSCBufferPerFrame.LightPosition = mSpotLight->Position();
		mPSCBufferPerFrame.SpotLightInnerAngle = mSpotLight->InnerAngle();
		mPSCBufferPerFrame.SpotLightOuterAngle = mSpotLight->OuterAngle();
		mPSCBufferPerFrame.LightColor = ColorHelper::ToFloat4(mSpotLight->Color(), true);
		mPSCBufferPerFrame.CameraPosition = mCamera->Position();


		// Locate possible input devices
		mKeyboard = static_cast<KeyboardComponent*>(mGame->Services().GetService(KeyboardComponent::TypeIdClass()));
		mGamePad = static_cast<GamePadComponent*>(mGame->Services().GetService(GamePadComponent::TypeIdClass()));
	}

	void SpotlightLightingDemo::Update(const GameTime & gameTime)
	{
		static float angle = 0.0f;

		if (mAnimationEnabled)
		{
			angle += gameTime.ElapsedGameTimeSeconds().count() * ModelRotationRate;
			XMStoreFloat4x4(&mWorldMatrix, XMMatrixRotationY(angle));
		}

		if (mKeyboard != nullptr && mKeyboard->WasKeyPressedThisFrame(Keys::Space))
		{
			ToggleAnimation();
		}

		if (mGamePad != nullptr)
		{
			auto& gamePadState = mGamePad->CurrentState();
			if (gamePadState.IsConnected())
			{
				if (mGamePad->WasButtonPressedThisFrame(GamePadButtons::Start))
				{
					ToggleGamePadControls();
				}

				if (mGamePad->WasButtonPressedThisFrame(GamePadButtons::LeftStick))
				{
					ToggleAnimation();
				}

				UpdateAmbientLight(gameTime);
				UpdateSpotLight(gameTime, gamePadState);
				UpdateSpecularLight(gameTime);
			}
		}

		mProxyModel->Update(gameTime);
	}

	void SpotlightLightingDemo::Draw(const GameTime & gameTime)
	{
		UNREFERENCED_PARAMETER(gameTime);
		assert(mCamera != nullptr);

		ID3D11DeviceContext* direct3DDeviceContext = mGame->Direct3DDeviceContext();
		direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		direct3DDeviceContext->IASetInputLayout(mInputLayout.Get());

		UINT stride = sizeof(VertexPositionTextureNormal);
		UINT offset = 0;
		direct3DDeviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
		direct3DDeviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		direct3DDeviceContext->VSSetShader(mVertexShader.Get(), nullptr, 0);
		direct3DDeviceContext->PSSetShader(mPixelShader.Get(), nullptr, 0);

		XMMATRIX worldMatrix = XMLoadFloat4x4(&mWorldMatrix);
		XMMATRIX wvp = worldMatrix * mCamera->ViewProjectionMatrix();
		XMStoreFloat4x4(&mVSCBufferPerObject.WorldViewProjection, XMMatrixTranspose(wvp));
		XMStoreFloat4x4(&mVSCBufferPerObject.World, XMMatrixTranspose(worldMatrix));

		mPSCBufferPerFrame.CameraPosition = mCamera->Position();

		direct3DDeviceContext->UpdateSubresource(mVSConstantBufferPO.Get(), 0, nullptr, &mVSCBufferPerObject, 0, 0);
		direct3DDeviceContext->UpdateSubresource(mPSConstantBufferPO.Get(), 0, nullptr, &mPSCBufferPerObject, 0, 0);
		direct3DDeviceContext->UpdateSubresource(mVSConstantBufferPF.Get(), 0, nullptr, &mVSCBufferPerFrame, 0, 0);
		direct3DDeviceContext->UpdateSubresource(mPSConstantBufferPF.Get(), 0, nullptr, &mPSCBufferPerFrame, 0, 0);

		ID3D11Buffer* VertexShaderBuffers[] = { mVSConstantBufferPF.Get(), mVSConstantBufferPO.Get() };
		direct3DDeviceContext->VSSetConstantBuffers(0, ARRAYSIZE(VertexShaderBuffers), VertexShaderBuffers);

		ID3D11Buffer* PixelShaderBuffers[] = { mPSConstantBufferPF.Get(), mPSConstantBufferPO.Get() };
		direct3DDeviceContext->PSSetConstantBuffers(0, ARRAYSIZE(PixelShaderBuffers), PixelShaderBuffers);

		direct3DDeviceContext->PSSetShaderResources(0, 1, mColorTexture.GetAddressOf());
		direct3DDeviceContext->PSSetSamplers(0, 1, SamplerStates::TrilinearWrap.GetAddressOf());

		direct3DDeviceContext->DrawIndexed(mIndexCount, 0, 0);

		mProxyModel->Draw(gameTime);

		mRenderStateHelper.SaveAll();

		mSpriteBatch->Begin();

		std::wostringstream helpLabel;

		helpLabel << "Ambient Intensity (+X/-A): " << mPSCBufferPerFrame.AmbientColor.x << "\n";
		helpLabel << L"Specular Intensity (+DPadLeft/-DPadRight): " << mPSCBufferPerObject.SpecularColor.x << "\n";
		helpLabel << L"Specular Power (+DPadUp/-DPadDown): " << mPSCBufferPerObject.SpecularPower << "\n";
		helpLabel << L"Spot Light Intensity (+Y/-B): " << mPSCBufferPerFrame.LightColor.x << "\n";
		helpLabel << L"Move Spot Light (LPad, Triggers(L-Y/R+Y))\n";
		helpLabel << L"Rotate Spot Light (RPad)\n";
		helpLabel << L"Spot Light Radius (+RightBumper/-LeftBumper): " << mSpotLight->Radius() << "\n";
		helpLabel << L"Spot Light Inner Angle(+KeysUp/-KeysDown): " << mSpotLight->InnerAngle() << "\n";
		helpLabel << L"Spot Light Outer Angle(+KeysRight/-KeysLeft): " << mSpotLight->OuterAngle() << "\n";

		mSpriteFont->DrawString(mSpriteBatch.get(), helpLabel.str().c_str(), mTextPosition);

		mSpriteBatch->End();

		mRenderStateHelper.RestoreAll();
	}

	void SpotlightLightingDemo::CreateVertexBuffer(const Mesh& mesh, ID3D11Buffer** vertexBuffer) const
	{
		const vector<XMFLOAT3>& sourceVertices = mesh.Vertices();
		const vector<XMFLOAT3>& sourceNormals = mesh.Normals();
		const auto& sourceUVs = mesh.TextureCoordinates().at(0);

		vector<VertexPositionTextureNormal> vertices;
		vertices.reserve(sourceVertices.size());
		for (UINT i = 0; i < sourceVertices.size(); i++)
		{
			const XMFLOAT3& position = sourceVertices.at(i);
			const XMFLOAT3& uv = sourceUVs->at(i);
			const XMFLOAT3& normal = sourceNormals.at(i);

			vertices.push_back(VertexPositionTextureNormal(XMFLOAT4(position.x, position.y, position.z, 1.0f), XMFLOAT2(uv.x, uv.y), normal));
		}
		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.ByteWidth = sizeof(VertexPositionTextureNormal) * static_cast<UINT>(vertices.size());
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData = { 0 };
		vertexSubResourceData.pSysMem = &vertices[0];
		ThrowIfFailed(mGame->Direct3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, vertexBuffer), "ID3D11Device::CreateBuffer() failed.");
	}

	void SpotlightLightingDemo::ToggleAnimation()
	{
		mAnimationEnabled = !mAnimationEnabled;
	}

	void SpotlightLightingDemo::UpdateAmbientLight(const GameTime& gameTime)
	{
		static float ambientIntensity = mPSCBufferPerFrame.AmbientColor.x;
		if (mKeyboard->WasKeyPressedThisFrame(Keys::NumPad0)) {
			DirectX::XMFLOAT4 r = ColorHelper::RandomColor();
			mPSCBufferPerFrame.AmbientColor = XMFLOAT4(r.x, r.y, r.z, 1.0f);
		}
		if (mGamePad->IsButtonDown(GamePadButtons::X) && ambientIntensity < 1.0f)
		{
			ambientIntensity += gameTime.ElapsedGameTimeSeconds().count();
			ambientIntensity = min(ambientIntensity, 1.0f);

			mPSCBufferPerFrame.AmbientColor = XMFLOAT4(ambientIntensity, ambientIntensity, ambientIntensity, 1.0f);
		}
		else if (mGamePad->IsButtonDown(GamePadButtons::A) && ambientIntensity > 0.0f)
		{
			ambientIntensity -= gameTime.ElapsedGameTimeSeconds().count();
			ambientIntensity = max(ambientIntensity, 0.0f);

			mPSCBufferPerFrame.AmbientColor = XMFLOAT4(ambientIntensity, ambientIntensity, ambientIntensity, 1.0f);
		}
	}

	void SpotlightLightingDemo::UpdateSpotLight(const Library::GameTime & gameTime, const DirectX::GamePad::State & gamePadState)
	{
		static float spotLightIntensity = 1.0f;
		float elapsedTime = static_cast<float>(gameTime.ElapsedGameTimeSeconds().count());

		if (mKeyboard->WasKeyPressedThisFrame(Keys::NumPad1)) {
			DirectX::XMFLOAT4 r = ColorHelper::RandomColor();
			mPSCBufferPerFrame.LightColor = XMFLOAT4(r.x, r.y, r.z, 1.0f);
		}

		// Update spot light intensity		
		if (mGamePad->IsButtonDown(GamePadButtons::Y) && spotLightIntensity < 1.0f)
		{
			spotLightIntensity += elapsedTime;
			spotLightIntensity = min(spotLightIntensity, 1.0f);

			mPSCBufferPerFrame.LightColor = XMFLOAT4(spotLightIntensity, spotLightIntensity, spotLightIntensity, 1.0f);
			mSpotLight->SetColor(mPSCBufferPerFrame.LightColor);

		}
		if (mGamePad->IsButtonDown(GamePadButtons::B) && spotLightIntensity > 0.0f)
		{
			spotLightIntensity -= elapsedTime;
			spotLightIntensity = max(spotLightIntensity, 0.0f);

			mPSCBufferPerFrame.LightColor = XMFLOAT4(spotLightIntensity, spotLightIntensity, spotLightIntensity, 1.0f);
			mSpotLight->SetColor(mPSCBufferPerFrame.LightColor);
		}

		// Move spot light
		if (mUseGamePadForSpotLight) {
			XMFLOAT3 movementAmount = Vector3Helper::Zero;
			movementAmount.x = gamePadState.thumbSticks.leftX;
			movementAmount.y = gamePadState.triggers.right - gamePadState.triggers.left;
			movementAmount.z = -gamePadState.thumbSticks.leftY;

			XMVECTOR movement = XMLoadFloat3(&movementAmount) * LightMovementRate * elapsedTime;
			mSpotLight->SetPosition(mSpotLight->PositionVector() + movement);
			mProxyModel->SetPosition(mSpotLight->Position());
			mVSCBufferPerFrame.LightPosition = mSpotLight->Position();
			mPSCBufferPerFrame.LightPosition = mSpotLight->Position();


			// Rotate spot light
			XMFLOAT2 rotationAmount = Vector2Helper::Zero;
			rotationAmount.x = gamePadState.thumbSticks.rightX * elapsedTime * LightRotationRate.x;
			rotationAmount.y = -gamePadState.thumbSticks.rightY * elapsedTime * LightRotationRate.y;

			XMMATRIX lightRotationMatrix = XMMatrixIdentity();
			if (rotationAmount.x != 0)
			{
				lightRotationMatrix = XMMatrixRotationAxis(mProxyModel->UpVector(), -rotationAmount.x);
			}

			if (rotationAmount.y != 0)
			{
				XMMATRIX lightRotationAxisMatrix = XMMatrixRotationAxis(mProxyModel->DirectionVector(), -rotationAmount.y);
				lightRotationMatrix *= lightRotationAxisMatrix;
			}

			if (rotationAmount.x != 0.0f || rotationAmount.y != 0.0f)
			{
				mSpotLight->ApplyRotation(lightRotationMatrix);
				mProxyModel->ApplyRotation(lightRotationMatrix);
				mVSCBufferPerFrame.LightLookAt = mSpotLight->Direction();
			}
		}
		// Update the light's radius
		if (mGamePad->IsButtonDown(GamePadButtons::RightShoulder))
		{
			float radius = mSpotLight->Radius() + LightModulationRate * elapsedTime;
			mSpotLight->SetRadius(radius);
			mVSCBufferPerFrame.LightRadius = mSpotLight->Radius();
		} else if (mGamePad->IsButtonDown(GamePadButtons::LeftShoulder))
		{
			float radius = mSpotLight->Radius() - LightModulationRate * elapsedTime;
			radius = max(radius, 0.0f);
			mSpotLight->SetRadius(radius);
			mVSCBufferPerFrame.LightRadius = mSpotLight->Radius();
		}

		// Update inner and outer angles
		static float innerAngle = mSpotLight->InnerAngle();
		if (mKeyboard->IsKeyDown(Keys::Up) && innerAngle < 1.0f)
		{
			innerAngle += elapsedTime;
			innerAngle = min(innerAngle, 1.0f);

			mSpotLight->SetInnerAngle(innerAngle);
			mPSCBufferPerFrame.SpotLightInnerAngle = mSpotLight->InnerAngle();
		} else if (mKeyboard->IsKeyDown(Keys::Down) && innerAngle > 0.5f)
		{
			innerAngle -= elapsedTime;
			innerAngle = max(innerAngle, 0.5f);

			mSpotLight->SetInnerAngle(innerAngle);
			mPSCBufferPerFrame.SpotLightInnerAngle = mSpotLight->InnerAngle();
		}

		static float outerAngle = mSpotLight->OuterAngle();
		if (mKeyboard->IsKeyDown(Keys::Right) && outerAngle < 0.5f)
		{
			outerAngle += elapsedTime;
			outerAngle = min(outerAngle, 0.5f);

			mSpotLight->SetOuterAngle(outerAngle);
			mPSCBufferPerFrame.SpotLightOuterAngle = mSpotLight->OuterAngle();
		}
		if (mKeyboard->IsKeyDown(Keys::Left) && outerAngle > 0.0f)
		{
			outerAngle -= elapsedTime;
			outerAngle = max(outerAngle, 0.0f);

			mSpotLight->SetOuterAngle(outerAngle);
			mPSCBufferPerFrame.SpotLightOuterAngle = mSpotLight->OuterAngle();
		}
	}

	void SpotlightLightingDemo::UpdateSpecularLight(const Library::GameTime & gameTime)
	{
		static float specularIntensity = 1.0f;

		if (mKeyboard->WasKeyPressedThisFrame(Keys::NumPad2)) {
			DirectX::XMFLOAT4 r = ColorHelper::RandomColor();
			mPSCBufferPerObject.SpecularColor = XMFLOAT3(r.x, r.y, r.z);
		}

		if (mGamePad->IsButtonDown(GamePadButtons::DPadLeft) && specularIntensity < 1.0f)
		{
			specularIntensity += static_cast<float>(gameTime.ElapsedGameTimeSeconds().count());
			specularIntensity = min(specularIntensity, 1.0f);

			mPSCBufferPerObject.SpecularColor = XMFLOAT3(specularIntensity, specularIntensity, specularIntensity);
		}
		else if (mGamePad->IsButtonDown(GamePadButtons::DPadRight) && specularIntensity > 0.0f)
		{
			specularIntensity -= (float)gameTime.ElapsedGameTimeSeconds().count();
			specularIntensity = max(specularIntensity, 0.0f);

			mPSCBufferPerObject.SpecularColor = XMFLOAT3(specularIntensity, specularIntensity, specularIntensity);
		}

		static float specularPower = mPSCBufferPerObject.SpecularPower;

		if (mGamePad->IsButtonDown(GamePadButtons::DPadDown) && specularPower < UCHAR_MAX)
		{
			specularPower += LightModulationRate * static_cast<float>(gameTime.ElapsedGameTimeSeconds().count());
			specularPower = min(specularPower, static_cast<float>(UCHAR_MAX));

			mPSCBufferPerObject.SpecularPower = specularPower;
		}

		if (mGamePad->IsButtonDown(GamePadButtons::DPadUp) && specularPower > 1.0f)
		{
			specularPower -= LightModulationRate * static_cast<float>(gameTime.ElapsedGameTimeSeconds().count());
			specularPower = max(specularPower, 1.0f);

			mPSCBufferPerObject.SpecularPower = specularPower;
		}
	}


	void SpotlightLightingDemo::ToggleGamePadControls()
	{
		mUseGamePadForSpotLight = !mUseGamePadForSpotLight;
		if (mCamera->Is(FirstPersonCamera::TypeIdClass()))
		{
			FirstPersonCamera* camera = mCamera->As<FirstPersonCamera>();
			if (camera != nullptr)
			{
				camera->SetGamePad(mUseGamePadForSpotLight ? nullptr : mGamePad);
			}
		}
	}
}