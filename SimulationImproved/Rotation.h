#pragma once
#include "IAnimate.h"
class Rotation : public IAnimate
{
public:
	Rotation(const DirectX::XMVECTOR& rotation, float speed) : _pRotation(rotation), _pSpeed(speed) {}
	void Animate(float deltaT, Object& object) override;

private:
	DirectX::XMVECTOR _pRotation;
	float _pSpeed;
};

