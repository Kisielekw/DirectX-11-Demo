#pragma once

#include "ICollider.h"

class BoxCollider : public ICollider
{
	DirectX::BoundingBox _boundingBox;

public:
	BoxCollider() = default;

	void setTransform(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& scale, const DirectX::XMVECTOR& rotation) override;
	void setBoundingBox(const DirectX::BoundingBox& boundingBox) { _boundingBox = boundingBox; }

	const DirectX::BoundingBox& getBoundingBox() const { return _boundingBox; }

	bool intersects(const ICollider& other) const override;

	bool intersectsWithBoundingBox(const DirectX::BoundingBox& box) const override;

	bool intersectsWithBoundingSphere(const DirectX::BoundingSphere& sphere) const override;
};

