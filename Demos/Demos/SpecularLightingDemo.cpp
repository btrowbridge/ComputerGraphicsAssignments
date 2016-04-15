#include "pch.h"
using namespace Library;
using namespace DirectX;
using namespace std;

namespace Rendering
{
	SpecularLightingDemo::SpecularLightingDemo(Library::Game & game) :
		DrawableGameComponent(game), mVertexShader(), mIndexCount(), mPixelShader(), mWorldMatrix(MatrixHelper::Identity), mAnimationEnabled(true)
	{
		XMStoreFloat4x4(&mWorldMatrix, XMMatrixIdentity());
	}
	SpecularLightingDemo::SpecularLightingDemo(Game & game, const shared_ptr<Camera>& camera) :
		DrawableGameComponent(game, camera), mWorldMatrix(MatrixHelper::Identity), mIndexCount(0), mAnimationEnabled(true)
	{
	}
	void SpecularLightingDemo::Initialize()
	{
		// Load a compiled vertex shader
		std::vector<char> compiledVertexShader;
		Utility::LoadBinaryFile(L"Content\\Shaders\\DiffuseLightingDemoVS.cso", compiledVertexShader);
		ThrowIfFailed(mGame->Direct3DDevice()->CreateVertexShader(&compiledVertexShader[0], compiledVertexShader.size(), nullptr, mVertexShader.ReleaseAndGetAddressOf()), "ID3D11Device::CreatedVertexShader() failed.");

		// Load a compiled pixel shader
		std::vector<char> compiledPixelShader;
		Utility::LoadBinaryFile(L"Content\\Shaders\\DiffuseLightingDemoPS.cso", compiledPixelShader);
		ThrowIfFailed(mGame->Direct3DDevice()->CreatePixelShader(&compiledPixelShader[0], compiledPixelShader.size(), nullptr, mPixelShader.ReleaseAndGetAddressOf()), "ID3D11Device::CreatedPixelShader() failed.");

		// Create an input layout
		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 }
		};

		ThrowIfFailed(mGame->Direct3DDevice()->CreateInputLayout(inputElementDescriptions, ARRAYSIZE(inputElementDescriptions), &compiledVertexShader[0], compiledVertexShader.size(), mInputLayout.ReleaseAndGetAddressOf()), "ID3D11Device::CreateInputLayout() failed.");

		//load model

		unique_ptr<Library::Model> model = make_unique<Library::Model>("Content\\Models\\Sphere.obj.bin");
		for (UINT i = 0; i < model->Meshes().size(); i++) {
			Mesh* mesh = model->Meshes().at(i).get();

			//vertex buffer
			CreateVertexBuffer(mGame->Direct3DDevice(), *mesh, mVertexBuffer.GetAddressOf());

			//index buffer

			mesh->CreateIndexBuffer(*mGame->Direct3DDevice(), mIndexBuffer.ReleaseAndGetAddressOf());
			mIndexCount += static_cast<UINT>(mesh->Indices().size());
		}

		//Constant Buffer perObject
		D3D11_BUFFER_DESC constantBufferDesc = { 0 };
		constantBufferDesc.ByteWidth = sizeof(CBufferPerObjectVS);
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ThrowIfFailed(mGame->Direct3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, mConstantBufferPerObjectVS.ReleaseAndGetAddressOf()), "ID3D11Device::CreateBuffer() failed.");

		//Constant Buffer per Object
		constantBufferDesc.ByteWidth = sizeof(CBufferPerObjectPS);
		ThrowIfFailed(mGame->Direct3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, mConstantBufferPerObjectPS.ReleaseAndGetAddressOf()), "ID3D11Device::CreateBuffer() failed.");

		//Constant Buffer per frame PS
		constantBufferDesc.ByteWidth = sizeof(CBufferPerFramePS);
		ThrowIfFailed(mGame->Direct3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, mConstantBufferPerFramePS.ReleaseAndGetAddressOf()), "ID3D11Device::CreateBuffer() failed.");

		//Constant Buffer per frame VS
		constantBufferDesc.ByteWidth = sizeof(CBufferPerFrameVS);
		ThrowIfFailed(mGame->Direct3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, mConstantBufferPerFrameVS.ReleaseAndGetAddressOf()), "ID3D11Device::CreateBuffer() failed.");

		//Load a texture
		wstring textureName = L"Content\\Textures\\EarthComposite.dds";
		ThrowIfFailed(CreateDDSTextureFromFile(mGame->Direct3DDevice(), textureName.c_str(), nullptr, mColorTexture.ReleaseAndGetAddressOf()), "CreateDDSTexture failed");

		//Lighting
		XMVECTOR ambientColor = { 0.5f,0.0f,0.0f,1.0f };
		XMVECTOR directionLight = { 1.0f,0.0f,1.0f, 0.0f };

		XMStoreFloat4(&mCBufferPerFramePS.AmbientColor, ambientColor);
		XMStoreFloat4(&mCBufferPerFramePS.DirectionLight, directionLight);

		mGame->Direct3DDeviceContext()->UpdateSubresource(mConstantBufferPerFramePS.Get(), 0, nullptr, &mCBufferPerFramePS, 0, 0);
	}
	void SpecularLightingDemo::SetDirectionLight(float x, float y, float z) {
		mCBufferPerFramePS.DirectionLight = XMFLOAT4(x, y, z, 0.0f);
		mGame->Direct3DDeviceContext()->UpdateSubresource(mConstantBufferPerFramePS.Get(), 0, nullptr, &mCBufferPerFramePS, 0, 0);
	}
	void SpecularLightingDemo::SetAmbientColor(float r, float b, float g, float a) {
		mCBufferPerFramePS.AmbientColor = XMFLOAT4(r, b, g, a);
		mGame->Direct3DDeviceContext()->UpdateSubresource(mConstantBufferPerFramePS.Get(), 0, nullptr, &mCBufferPerFramePS, 0, 0);
	}
	void SpecularLightingDemo::SetAnimationEnabled(bool isEnabled)
	{
		mAnimationEnabled = isEnabled;
	}

	bool SpecularLightingDemo::AnimationEnabled()
	{
		return mAnimationEnabled;
	}

	void SpecularLightingDemo::CreateVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const {
		const std::vector < XMFLOAT3 >& sourceVertices = mesh.Vertices();
		const std::vector < XMFLOAT3 >& sourceNormals = mesh.Normals();
		const auto& sourceUVs = mesh.TextureCoordinates().at(0);

		std::vector <VertexPositionTextureNormal> vertices;
		vertices.reserve(sourceVertices.size());
		for (UINT i = 0; i < sourceVertices.size(); i++) {
			const XMFLOAT3& position = sourceVertices.at(i);
			const XMFLOAT3& uv = sourceUVs->at(i);
			const XMFLOAT3& normal = sourceNormals.at(i);
			vertices.push_back(VertexPositionTextureNormal(XMFLOAT4(position.x, position.y, position.z, 1.0f), XMFLOAT2(uv.x, uv.y), normal));
		}
		//Vertex Buffer
		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.ByteWidth = sizeof(VertexPositionTextureNormal) * static_cast<UINT>(vertices.size());
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData = { 0 };
		vertexSubResourceData.pSysMem = &vertices[0];

		ThrowIfFailed(device->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, vertexBuffer), "ID3D11Device::CreateBuffer() failed.");
	}

	void SpecularLightingDemo::Draw(const Library::GameTime & gameTime)
	{
		UNREFERENCED_PARAMETER(gameTime);

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
		worldMatrix = XMMatrixTranspose(worldMatrix);
		XMStoreFloat4x4(&mCBufferPerObjectVS.World, worldMatrix);
		XMStoreFloat4x4(&mCBufferPerObjectVS.WorldViewProjection, wvp);


		//Should combine to an array of shader resources


		direct3DDeviceContext->VSSetConstantBuffers(0, 2, mConstantBufferPerObjectVS.GetAddressOf());
		direct3DDeviceContext->VSSetConstantBuffers(1, 2, mConstantBufferPerFrameVS.GetAddressOf());

		direct3DDeviceContext->PSSetShaderResources(0, 1, mColorTexture.GetAddressOf());
		direct3DDeviceContext->PSSetSamplers(0, 1, SamplerStates::TrilinearMirror.ReleaseAndGetAddressOf());

		direct3DDeviceContext->PSSetConstantBuffers(0, 2, mConstantBufferPerObjectPS.GetAddressOf());
		direct3DDeviceContext->PSSetConstantBuffers(1, 2, mConstantBufferPerFramePS.GetAddressOf());

		direct3DDeviceContext->DrawIndexed(mIndexCount, 0, 0);
	}

	void SpecularLightingDemo::Update(const Library::GameTime & gameTime)
	{
		if (mAnimationEnabled) {
			const float rateOfChange = 30.0f;
			static float yRotation = 0.0f;

			yRotation -= rateOfChange * gameTime.ElapsedGameTimeSeconds().count();

			XMStoreFloat4x4(&mWorldMatrix, XMMatrixRotationY(XMConvertToRadians(yRotation)));
		}
	}
}