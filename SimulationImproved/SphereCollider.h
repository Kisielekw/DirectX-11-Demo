#pragma once
#include "ICollider.h"

class SphereCollider : public ICollider
{
	DirectX::BoundingSphere _boundingSphere;

public:
	SphereCollider() = default;

	void setTransform(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& scale, const DirectX::XMVECTOR& rotation) override;

	bool intersects(const ICollider& other) const override;

	bool intersectsWithBoundingBox(const DirectX::BoundingBox& box) const override;

	bool intersectsWithBoundingSphere(const DirectX::BoundingSphere& sphere) const override;
};

