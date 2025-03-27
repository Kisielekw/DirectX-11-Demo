#include "BoxCollider.h"

bool BoxCollider::intersects(const ICollider& other) const
{
	return other.intersectsWithBoundingBox(_boundingBox);
}

bool BoxCollider::intersectsWithBoundingBox(const DirectX::BoundingBox& box) const
{
	return _boundingBox.Intersects(box);
}

bool BoxCollider::intersectsWithBoundingSphere(const DirectX::BoundingSphere& sphere) const
{
	return _boundingBox.Intersects(sphere);
}

void BoxCollider::setTransform(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& scale, const DirectX::XMVECTOR& rotation)
{
	DirectX::XMFLOAT3 newPosition;
	DirectX::XMStoreFloat3(&newPosition, pos);
	_boundingBox.Center = newPosition;

	// Update the bounding box's extents based on scale
	DirectX::XMFLOAT3 newScale;
	DirectX::XMStoreFloat3(&newScale, scale);
	_boundingBox.Extents = { newScale.x, newScale.y, newScale.z };
}