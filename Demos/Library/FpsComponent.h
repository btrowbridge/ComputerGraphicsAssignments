#pragma once

#include "DrawableGameComponent.h"
#include <DirectXMath.h>
#include <chrono>
#include <memory>

namespace DirectX
{
    class SpriteBatch;
    class SpriteFont;
}

namespace Library
{
    class FpsComponent final : public Library::DrawableGameComponent
    {
        RTTI_DECLARATIONS(FpsComponent, DrawableGameComponent)

    public:
        FpsComponent(Game& game);

		FpsComponent() = delete;
		FpsComponent(const FpsComponent& rhs) = delete;
		FpsComponent& operator=(const FpsComponent& rhs) = delete;

        DirectX::XMFLOAT2& TextPosition();
        int FrameRate() const;

        virtual void Initialize() override;
        virtual void Update(const Library::GameTime& gameTime) override;
        virtual void Draw(const Library::GameTime& gameTime) override;

    private:        
		std::unique_ptr<DirectX::SpriteBatch> mSpriteBatch;
		std::unique_ptr<DirectX::SpriteFont> mSpriteFont;
		DirectX::XMFLOAT2 mTextPosition;

        int mFrameCount;
        int mFrameRate;
		std::chrono::milliseconds mLastTotalGameTime;
    };
}