#include "Translate.h"
#include "Object.h"

void Translate::Animate(float deltaT, Object& object)
{
	auto position = object.position();
	position = DirectX::XMVectorSubtract(position, _pCurrentMovement);
	_pCurrentMovement = DirectX::XMVectorScale(_pTranslation, sinf(_pTime));
	position = DirectX::XMVectorAdd(position, _pCurrentMovement);
	object.setPosition(position);
	_pTime += deltaT * _pSpeed;
}
