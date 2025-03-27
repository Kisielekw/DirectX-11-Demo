#pragma once
#include <DirectXCollision.h>
#include <DirectXMath.h>

class ICollider
{
public:
	virtual ~ICollider() = default;

	virtual void setTransform(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& scale, const DirectX::XMVECTOR& rotation) = 0;

	virtual bool intersects(const ICollider& other) const = 0;
	virtual bool intersectsWithBoundingBox(const DirectX::BoundingBox& box) const = 0;
	virtual bool intersectsWithBoundingSphere(const DirectX::BoundingSphere& sphere) const = 0;
};

