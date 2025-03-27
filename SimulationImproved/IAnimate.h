#pragma once
#include <DirectXMath.h>
class Object;

class IAnimate
{
public:
	virtual ~IAnimate() = default;
	virtual void Animate(float deltaT, Object& object) = 0;
};

