#include "pch.h"

using namespace std;
using namespace Library;
using namespace DirectX;

namespace Rendering
{
	const float SpecularLightingDemo::ModelRotationRate = XM_PI;
	const XMFLOAT2 SpecularLightingDemo::LightRotationRate = XMFLOAT2(XMConvertToRadians(500.0f), XMConvertToRadians(500.0f));
	const float SpecularLightingDemo::LightModulationRate = UCHAR_MAX;

	SpecularLightingDemo::SpecularLightingDemo(Game& game) :
		DrawableGameComponent(game), mWorldMatrix(MatrixHelper::Identity), mIndexCount(0),
		mAnimationEnabled(false), mUseGamePadForDirectionalLight(false)
	{
	}

	SpecularLightingDemo::SpecularLightingDemo(Game & game, const shared_ptr<Camera>& camera) :
		DrawableGameComponent(game, camera), mWorldMatrix(MatrixHelper::Identity), mIndexCount(0),
		mAnimationEnabled(false), mUseGamePadForDirectionalLight(false)
	{
	}

	bool SpecularLightingDemo::AnimationEnabled() const
	{
		return mAnimationEnabled;
	}

	void SpecularLightingDemo::SetAnimationEnabled(bool enabled)
	{
		mAnimationEnabled = enabled;
	}

	void SpecularLightingDemo::Initialize()
	{
		// Load a compiled vertex shader
		vector<char> compiledVertexShader;
		Utility::LoadBinaryFile(L"Content\\Shaders\\SpecularLightingDemoVS.cso", compiledVertexShader);
		ThrowIfFailed(mGame->Direct3DDevice()->CreateVertexShader(&compiledVertexShader[0], compiledVertexShader.size(), nullptr, mVertexShader.ReleaseAndGetAddressOf()), "ID3D11Device::CreatedVertexShader() failed.");

		// Load a compiled pixel shader
		vector<char> compiledPixelShader;
		Utility::LoadBinaryFile(L"Content\\Shaders\\SpecularLightingDemoPS.cso", compiledPixelShader);
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

		// Update the pixel shader constant buffer
		mGame->Direct3DDeviceContext()->UpdateSubresource(mPSConstantBufferPF.Get(), 0, nullptr, &mPSCBufferPerFrame, 0, 0);

		mDirectionalLight = std::make_unique<DirectionalLight>(*mGame);
		mPSCBufferPerFrame.LightDirection = mDirectionalLight->DirectionToLight();

		mPSCBufferPerObject.SpecularPower = 25.0f;

		XMStoreFloat3(&mPSCBufferPerObject.SpecularColor, DirectX::Colors::White);
		XMStoreFloat4(&mPSCBufferPerFrame.AmbientColor, DirectX::Colors::Black);
		XMStoreFloat4(&mPSCBufferPerFrame.LightColor, DirectX::Colors::White);

		// Load a proxy model for the directional light
		mProxyModel = make_unique<ProxyModel>(*mGame, mCamera, "Content\\Models\\DirectionalLightProxy.obj.bin", 0.5f);
		mProxyModel->Initialize();
		mProxyModel->SetPosition(10.0f, 0.0, 0.0f);
		mProxyModel->ApplyRotation(XMMatrixRotationY(XM_PIDIV2));

		// Locate possible input devices
		mKeyboard = static_cast<KeyboardComponent*>(mGame->Services().GetService(KeyboardComponent::TypeIdClass()));
		mGamePad = static_cast<GamePadComponent*>(mGame->Services().GetService(GamePadComponent::TypeIdClass()));
	}

	void SpecularLightingDemo::Update(const GameTime & gameTime)
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
				UpdateSpecularLight(gameTime);
				UpdateDirectionalLight(gameTime, gamePadState);
			}
		}

		mProxyModel->Update(gameTime);
	}

	void SpecularLightingDemo::Draw(const GameTime & gameTime)
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
		wvp = XMMatrixTranspose(wvp);
		XMStoreFloat4x4(&mVSCBufferPerObject.WorldViewProjection, wvp);
		XMStoreFloat4x4(&mVSCBufferPerObject.World, XMMatrixTranspose(worldMatrix));

		mVSCBufferPerFrame.CameraPosition = mCamera->Position();

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
	}

	void SpecularLightingDemo::CreateVertexBuffer(const Mesh& mesh, ID3D11Buffer** vertexBuffer) const
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

	void SpecularLightingDemo::ToggleAnimation()
	{
		mAnimationEnabled = !mAnimationEnabled;
	}

	void SpecularLightingDemo::UpdateAmbientLight(const GameTime& gameTime)
	{
		static float ambientIntensity = mPSCBufferPerFrame.AmbientColor.x;

		if (mGamePad->IsButtonDown(GamePadButtons::A) && ambientIntensity < 1.0f)
		{
			ambientIntensity += gameTime.ElapsedGameTimeSeconds().count();
			ambientIntensity = min(ambientIntensity, 1.0f);

			mPSCBufferPerFrame.AmbientColor = XMFLOAT4(ambientIntensity, ambientIntensity, ambientIntensity, 1.0f);
			mGame->Direct3DDeviceContext()->UpdateSubresource(mPSConstantBufferPF.Get(), 0, nullptr, &mPSCBufferPerFrame, 0, 0);
		}
		else if (mGamePad->IsButtonDown(GamePadButtons::X) && ambientIntensity > 0.0f)
		{
			ambientIntensity -= gameTime.ElapsedGameTimeSeconds().count();
			ambientIntensity = max(ambientIntensity, 0.0f);

			mPSCBufferPerFrame.AmbientColor = XMFLOAT4(ambientIntensity, ambientIntensity, ambientIntensity, 1.0f);
			mGame->Direct3DDeviceContext()->UpdateSubresource(mPSConstantBufferPF.Get(), 0, nullptr, &mPSCBufferPerFrame, 0, 0);
		}
	}

	void SpecularLightingDemo::UpdateDirectionalLight(const GameTime& gameTime, const GamePad::State& gamePadState)
	{
		static float directionalIntensity = mPSCBufferPerFrame.LightColor.x;
		float elapsedTime = gameTime.ElapsedGameTimeSeconds().count();

		// Update directional light intensity
		if (mGamePad->IsButtonDown(GamePadButtons::B) && directionalIntensity < 1.0f)
		{
			directionalIntensity += elapsedTime;
			directionalIntensity = min(directionalIntensity, 1.0f);

			mPSCBufferPerFrame.LightColor = XMFLOAT4(directionalIntensity, directionalIntensity, directionalIntensity, 1.0f);
			mDirectionalLight->SetColor(mPSCBufferPerFrame.LightColor);
			mGame->Direct3DDeviceContext()->UpdateSubresource(mPSConstantBufferPF.Get(), 0, nullptr, &mPSCBufferPerFrame, 0, 0);
		}
		else if (mGamePad->IsButtonDown(GamePadButtons::Y) && directionalIntensity > 0.0f)
		{
			directionalIntensity -= elapsedTime;
			directionalIntensity = max(directionalIntensity, 0.0f);

			mPSCBufferPerFrame.LightColor = XMFLOAT4(directionalIntensity, directionalIntensity, directionalIntensity, 1.0f);
			mDirectionalLight->SetColor(mPSCBufferPerFrame.LightColor);
			mGame->Direct3DDeviceContext()->UpdateSubresource(mPSConstantBufferPF.Get(), 0, nullptr, &mPSCBufferPerFrame, 0, 0);
		}

		if (mUseGamePadForDirectionalLight)
		{
			// Rotate directional light
			XMFLOAT2 rotationAmount;
			rotationAmount.x = -gamePadState.thumbSticks.rightX * LightRotationRate.x * elapsedTime;
			rotationAmount.y = gamePadState.thumbSticks.rightY * LightRotationRate.y * elapsedTime;

			XMMATRIX lightRotationMatrix = XMMatrixIdentity();
			if (rotationAmount.x != 0)
			{
				lightRotationMatrix = XMMatrixRotationY(rotationAmount.x);
			}

			if (rotationAmount.y != 0)
			{
				XMMATRIX lightRotationAxisMatrix = XMMatrixRotationAxis(mDirectionalLight->RightVector(), rotationAmount.y);
				lightRotationMatrix *= lightRotationAxisMatrix;
			}

			if (rotationAmount.x != 0.0f || rotationAmount.y != 0.0f)
			{
				mDirectionalLight->ApplyRotation(lightRotationMatrix);
				mProxyModel->ApplyRotation(lightRotationMatrix);
				mPSCBufferPerFrame.LightDirection = mDirectionalLight->DirectionToLight();
				mGame->Direct3DDeviceContext()->UpdateSubresource(mPSConstantBufferPF.Get(), 0, nullptr, &mPSCBufferPerFrame, 0, 0);
			}
		}
	}

	void SpecularLightingDemo::UpdateSpecularLight(const Library::GameTime & gameTime)
	{
		static float specularIntensity = 1.0f;

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

	void SpecularLightingDemo::ToggleGamePadControls()
	{
		mUseGamePadForDirectionalLight = !mUseGamePadForDirectionalLight;
		if (mCamera->Is(FirstPersonCamera::TypeIdClass()))
		{
			FirstPersonCamera* camera = mCamera->As<FirstPersonCamera>();
			if (camera != nullptr)
			{
				camera->SetGamePad(mUseGamePadForDirectionalLight ? nullptr : mGamePad);
			}
		}
	}
}