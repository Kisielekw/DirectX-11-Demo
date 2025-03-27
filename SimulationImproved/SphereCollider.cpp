#include "SphereCollider.h"

void SphereCollider::setTransform(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& scale, const DirectX::XMVECTOR& rotation)
{
	DirectX::XMFLOAT3 newPosition;
	DirectX::XMStoreFloat3(&newPosition, pos);
	_boundingSphere.Center = newPosition;

	// Update the bounding sphere's radius based on scale
	_boundingSphere.Radius = scale.m128_f32[0];

}

bool SphereCollider::intersects(const ICollider& other) const
{
	return other.intersectsWithBoundingSphere(_boundingSphere);
}

bool SphereCollider::intersectsWithBoundingBox(const DirectX::BoundingBox& box) const
{
	return box.Intersects(_boundingSphere);
}

bool SphereCollider::intersectsWithBoundingSphere(const DirectX::BoundingSphere& sphere) const
{
	return sphere.Intersects(_boundingSphere);
}