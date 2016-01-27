#include "pch.h"
#include "PointDemo.h"
#include "Utility.h"



namespace Rendering {
	RTTI_DEFINITIONS(PointDemo)



	void PointDemo::Initialize()
	{
		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

		//Load compiled vertex shader
		std::vector<char> compiledVertexShader;
		Utility::LoadBinaryFile(L"Content\\Shaders\\VertexShader.cso", compiledVertexShader);
		ThrowIfFailed(mGame->Direct3DDevice()->CreateVertexShader(&compiledVertexShader[0], compiledVertexShader.size(), nullptr, &mVertexShader), "ID3D11VertexShader->CreateVertexShader: Failed");

		//Load compiled pixel shader
		std::vector<char> compiledPixelShader;
		Utility::LoadBinaryFile(L"Content\\Shaders\\PixelShader.cso", compiledPixelShader);
		ThrowIfFailed(mGame->Direct3DDevice()->CreatePixelShader(&compiledPixelShader[0], compiledPixelShader.size(), nullptr, &mPixelShader), "ID3D11PixelShader->CreatePixelShader: Failed");

		GameComponent::Initialize();
	}

	void PointDemo::Draw(const GameTime & gameTime)

	{
		ID3D11DeviceContext* direct3DDeviceContext = mGame->Direct3DDeviceContext();
		direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		direct3DDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
		direct3DDeviceContext->PSSetShader(mPixelShader, nullptr, 0);

		
		if (Visible()) {
			direct3DDeviceContext->Draw(1, 0);
		}
		UNREFERENCED_PARAMETER(gameTime);
		//DrawableGameComponent::Draw(gameTime);
	}


	PointDemo::PointDemo(Game & game/*, Camera & camera*/) 
		:DrawableGameComponent(game/*,camera*/), mVertexShader(nullptr), mPixelShader(nullptr)
	{
		
	}


}