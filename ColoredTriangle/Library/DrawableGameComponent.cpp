#include "pch.h"

namespace Library
{
	RTTI_DEFINITIONS(DrawableGameComponent)

	DrawableGameComponent::DrawableGameComponent()
		: GameComponent(), mVisible(true)
	{
	}

	DrawableGameComponent::DrawableGameComponent(Game& game)
		: GameComponent(game), mVisible(true)
	{
	}

	DrawableGameComponent::~DrawableGameComponent()
	{
	}

	bool DrawableGameComponent::Visible() const
	{
		return mVisible;
	}

	void DrawableGameComponent::SetVisible(bool visible)
	{
		mVisible = visible;
	}

	void DrawableGameComponent::Draw(const GameTime& gameTime)
	{
		UNREFERENCED_PARAMETER(gameTime);
	}
}