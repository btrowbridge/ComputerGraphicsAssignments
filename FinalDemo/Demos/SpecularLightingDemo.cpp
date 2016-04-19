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
		mAnimationEnabled(false), mUseGamePadForDirectionalLight(false), mRenderStateHelper(game), mSpriteBatch(nullptr), mSpriteFont(nullptr), mTextPosition(0.0f, 40.0f)
	{
	}

	SpecularLightingDemo::SpecularLightingDemo(Game & game, const shared_ptr<Camera>& camera) :
		DrawableGameComponent(game, camera), mWorldMatrix(MatrixHelper::Identity), mIndexCount(0),
		mAnimationEnabled(false), mUseGamePadForDirectionalLight(false), mRenderStateHelper(game), mSpriteBatch(nullptr), mSpriteFont(nullptr), mTextPosition(0.0f, 40.0f)
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

		mSpriteBatch = std::make_unique<SpriteBatch>(mGame->Direct3DDeviceContext());
		mSpriteFont = std::make_unique<SpriteFont>(mGame->Direct3DDevice(), L"Content\\Fonts\\Arial_14_Regular.spritefont");

		//Create DirectionalLight
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

		mRenderStateHelper.SaveAll();

		mSpriteBatch->Begin();

		std::wostringstream helpLabel;

		helpLabel << L"Specular Power (+DpadDown/-DpadUp): " << mPSCBufferPerObject.SpecularPower << "\n";
		helpLabel << L"Directional Light Intensity (+B/-Y): " << mPSCBufferPerFrame.LightColor.x << "\n";
		helpLabel << L"Rotate Directional Light (RStick)";
		helpLabel << L"All Colors RGB (+num/-D)\n";
		helpLabel << L"Ambient Color RGB (1,2,3) All/Intensity(+A/-X): ";
		helpLabel << L"( " << mPSCBufferPerFrame.AmbientColor.x << L", " << mPSCBufferPerFrame.AmbientColor.y << L", " << mPSCBufferPerFrame.AmbientColor.z << L")\n";
		helpLabel << L"Light Color RGB (4,5,6) All/Intensity(+Y/-B): ";
		helpLabel << L"( " << mPSCBufferPerFrame.LightColor.x << L", " << mPSCBufferPerFrame.LightColor.y << L", " << mPSCBufferPerFrame.LightColor.z << L")\n";
		helpLabel << L"Specular Color RGB (7,8,9) All/Intensity(+DPadLeft/-DPadRight): ";
		helpLabel << L"( " << mPSCBufferPerObject.SpecularColor.x << L", " << mPSCBufferPerObject.SpecularColor.y << L", " << mPSCBufferPerObject.SpecularColor.z << L")\n";

		mSpriteFont->DrawString(mSpriteBatch.get(), helpLabel.str().c_str(), mTextPosition);

		mSpriteBatch->End();

		mRenderStateHelper.RestoreAll();
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
		float elapsedTime = gameTime.ElapsedGameTimeSeconds().count();
		static float ambientr = mPSCBufferPerFrame.AmbientColor.x;
		static float ambientg = mPSCBufferPerFrame.AmbientColor.y;
		static float ambientb = mPSCBufferPerFrame.AmbientColor.z;

		//Color
		if (mKeyboard->IsKeyDown(Keys::NumPad1) || mGamePad->IsButtonDown(GamePadButtons::A)) {
			ambientr = max(0.0f, min(ambientr + elapsedTime, 1.0f));
			mPSCBufferPerFrame.AmbientColor = XMFLOAT4(ambientr, ambientg, ambientb, 1.0f);
		}
		else if (mKeyboard->IsKeyDown(Keys::D1) || mGamePad->IsButtonDown(GamePadButtons::X)) {
			ambientr = max(0.0f, min(ambientr - elapsedTime, 1.0f));
			mPSCBufferPerFrame.AmbientColor = XMFLOAT4(ambientr, ambientg, ambientb, 1.0f);
		}
		if (mKeyboard->IsKeyDown(Keys::NumPad2) || mGamePad->IsButtonDown(GamePadButtons::A)) {
			ambientg = max(0.0f, min(ambientg + elapsedTime, 1.0f));
			mPSCBufferPerFrame.AmbientColor = XMFLOAT4(ambientr, ambientg, ambientb, 1.0f);
		}
		else if (mKeyboard->IsKeyDown(Keys::D2) || mGamePad->IsButtonDown(GamePadButtons::X)) {
			ambientg = max(0.0f, min(ambientg - elapsedTime, 1.0f));
			mPSCBufferPerFrame.AmbientColor = XMFLOAT4(ambientr, ambientg, ambientb, 1.0f);
		}
		if (mKeyboard->IsKeyDown(Keys::NumPad3) || mGamePad->IsButtonDown(GamePadButtons::A)) {
			ambientb = max(0.0f, min(ambientb + elapsedTime, 1.0f));
			mPSCBufferPerFrame.AmbientColor = XMFLOAT4(ambientr, ambientg, ambientb, 1.0f);
		}
		else if (mKeyboard->IsKeyDown(Keys::D3) || mGamePad->IsButtonDown(GamePadButtons::X)) {
			ambientb = max(0.0f, min(ambientb - elapsedTime, 1.0f));
			mPSCBufferPerFrame.AmbientColor = XMFLOAT4(ambientr, ambientg, ambientb, 1.0f);
		}
	}

	void SpecularLightingDemo::UpdateDirectionalLight(const GameTime& gameTime, const GamePad::State& gamePadState)
	{
		float elapsedTime = gameTime.ElapsedGameTimeSeconds().count();
		static float lightr = mPSCBufferPerFrame.LightColor.x;
		static float lightg = mPSCBufferPerFrame.LightColor.y;
		static float lightb = mPSCBufferPerFrame.LightColor.z;

		//Color
		if (mKeyboard->IsKeyDown(Keys::NumPad4) || mGamePad->IsButtonDown(GamePadButtons::Y)) {
			lightr = max(0.0f, min(lightr + elapsedTime, 1.0f));
			mPSCBufferPerFrame.LightColor = XMFLOAT4(lightr, lightg, lightb, 1.0f);
		}
		else if (mKeyboard->IsKeyDown(Keys::D4) || mGamePad->IsButtonDown(GamePadButtons::B)) {
			lightr = max(0.0f, min(lightr - elapsedTime, 1.0f));
			mPSCBufferPerFrame.LightColor = XMFLOAT4(lightr, lightg, lightb, 1.0f);
		}
		if (mKeyboard->IsKeyDown(Keys::NumPad5) || mGamePad->IsButtonDown(GamePadButtons::Y)) {
			lightg = max(0.0f, min(lightg + elapsedTime, 1.0f));
			mPSCBufferPerFrame.LightColor = XMFLOAT4(lightr, lightg, lightb, 1.0f);
		}
		else if (mKeyboard->IsKeyDown(Keys::D5) || mGamePad->IsButtonDown(GamePadButtons::B)) {
			lightg = max(0.0f, min(lightg - elapsedTime, 1.0f));
			mPSCBufferPerFrame.LightColor = XMFLOAT4(lightr, lightg, lightb, 1.0f);
		}
		if (mKeyboard->IsKeyDown(Keys::NumPad6) || mGamePad->IsButtonDown(GamePadButtons::Y)) {
			lightb = max(0.0f, min(lightb + elapsedTime, 1.0f));
			mPSCBufferPerFrame.LightColor = XMFLOAT4(lightr, lightg, lightb, 1.0f);
		}
		else if (mKeyboard->IsKeyDown(Keys::D6) || mGamePad->IsButtonDown(GamePadButtons::B)) {
			lightb = max(0.0f, min(lightb - elapsedTime, 1.0f));
			mPSCBufferPerFrame.LightColor = XMFLOAT4(lightr, lightg, lightb, 1.0f);
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
		float elapsedTime = gameTime.ElapsedGameTimeSeconds().count();
		static float specr = mPSCBufferPerObject.SpecularColor.x;
		static float specg = mPSCBufferPerObject.SpecularColor.y;
		static float specb = mPSCBufferPerObject.SpecularColor.z;

		//Color
		if (mKeyboard->IsKeyDown(Keys::NumPad7) || mGamePad->IsButtonDown(GamePadButtons::DPadRight)) {
			specr = max(0.0f, min(specr + elapsedTime, 1.0f));
			mPSCBufferPerObject.SpecularColor = XMFLOAT3(specr, specg, specb);
		}
		else if (mKeyboard->IsKeyDown(Keys::D7) || mGamePad->IsButtonDown(GamePadButtons::DPadLeft)) {
			specr = max(0.0f, min(specr - elapsedTime, 1.0f));
			mPSCBufferPerObject.SpecularColor = XMFLOAT3(specr, specg, specb);
		}
		if (mKeyboard->IsKeyDown(Keys::NumPad8) || mGamePad->IsButtonDown(GamePadButtons::DPadRight)) {
			specg = max(0.0f, min(specg + elapsedTime, 1.0f));
			mPSCBufferPerObject.SpecularColor = XMFLOAT3(specr, specg, specb);
		}
		else if (mKeyboard->IsKeyDown(Keys::D8) || mGamePad->IsButtonDown(GamePadButtons::DPadLeft)) {
			specg = max(0.0f, min(specg - elapsedTime, 1.0f));
			mPSCBufferPerObject.SpecularColor = XMFLOAT3(specr, specg, specb);
		}
		if (mKeyboard->IsKeyDown(Keys::NumPad9) || mGamePad->IsButtonDown(GamePadButtons::DPadRight)) {
			specb = max(0.0f, min(specb + elapsedTime, 1.0f));
			mPSCBufferPerObject.SpecularColor = XMFLOAT3(specr, specg, specb);
		}
		else if (mKeyboard->IsKeyDown(Keys::D9) || mGamePad->IsButtonDown(GamePadButtons::DPadLeft)) {
			specb = max(0.0f, min(specb - elapsedTime, 1.0f));
			mPSCBufferPerObject.SpecularColor = XMFLOAT3(specr, specg, specb);
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