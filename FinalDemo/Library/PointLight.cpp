#include "pch.h"

using namespace DirectX;

namespace Library
{
	RTTI_DEFINITIONS(PointLight)

		const float PointLight::DefaultRadius = 10.0f;

	PointLight::PointLight(Game& game) :
		Light(game), mPosition(Vector3Helper::Zero), mRadius(DefaultRadius)
	{
	}

	XMFLOAT3& PointLight::Position()
	{
		return mPosition;
	}

	XMVECTOR PointLight::PositionVector() const
	{
		return XMLoadFloat3(&mPosition);
	}

	float PointLight::Radius() const
	{
		return mRadius;
	}

	void PointLight::SetPosition(float x, float y, float z)
	{
		XMVECTOR position = XMVectorSet(x, y, z, 1.0f);
		SetPosition(position);
	}

	void PointLight::SetPosition(FXMVECTOR position)
	{
		XMStoreFloat3(&mPosition, position);
	}

	void PointLight::SetPosition(const XMFLOAT3& position)
	{
		mPosition = position;
	}

	void PointLight::SetRadius(float value)
	{
		mRadius = value;
	}
}