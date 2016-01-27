#pragma once
#include "DrawableGameComponent.h"
#include "pch.h"

using namespace Library;
namespace Rendering {

	class PointDemo : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(PointDemo, DrawableGameComponent)
	public:
		PointDemo(Game& game/*, Camera& camera*/);
		~PointDemo() = default;

		PointDemo() = default;
		PointDemo(const PointDemo& rhs) = delete;
		PointDemo& operator=(const PointDemo& rhs) = delete;


		virtual void Initialize() override;
		virtual void Draw(const GameTime& gameTime) override;

	private:
		ID3D11VertexShader* mVertexShader;
		ID3D11PixelShader* mPixelShader;
	};

}