#include "Rotation.h"
#include "Object.h"

void Rotation::Animate(float deltaT, Object& object)
{
	auto rotation = object.rotation();
	rotation = DirectX::XMVectorAdd(rotation, DirectX::XMVectorScale(_pRotation, _pSpeed * deltaT));
	object.setRotation(rotation);
}
