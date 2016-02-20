#pragma once

#include "GameComponent.h"

namespace Library
{
    class Camera;

    class DrawableGameComponent : public GameComponent
    {
        RTTI_DECLARATIONS(DrawableGameComponent, GameComponent)

    public:
        DrawableGameComponent();
        DrawableGameComponent(Game& game);
        virtual ~DrawableGameComponent();

		DrawableGameComponent(const DrawableGameComponent& rhs) = delete;
		DrawableGameComponent& operator=(const DrawableGameComponent& rhs) = delete;

        bool Visible() const;
        void SetVisible(bool visible);

        virtual void Draw(const GameTime& gameTime);

    protected:
        bool mVisible;
    };
}