#pragma once
#include "IAnimate.h"
class Translate : public IAnimate
{
public:
	Translate(const DirectX::XMVECTOR& translation, float speed) : _pTranslation(translation), _pSpeed(speed) {}
	void Animate(float deltaT, Object& object) override;

private:
	DirectX::XMVECTOR _pCurrentMovement {0, 0, 0};
	DirectX::XMVECTOR _pTranslation;
	float _pSpeed = 1.0f;
	float _pTime = 0.0f;
};

