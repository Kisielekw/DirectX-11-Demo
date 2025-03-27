#include "Player.h"

#include <algorithm>

void Player::update(const float deltaTime, const std::bitset<256>& keys, const std::vector<std::shared_ptr<Object>>&
                    objects)
{
	auto position = _pPlayerObject->position();
	auto potentialVelocity = DirectX::XMVectorSet(0, 0, 0, 0);

	if (keys['S'])
		_pLookAt -= 1 * deltaTime;
	if (keys['W'])
		_pLookAt += 1 * deltaTime;

	_pLookAt = std::clamp(_pLookAt, 0.0f, DirectX::XM_PIDIV2);
	if (_pOnGround) 
	{
		DirectX::XMVECTOR moveDirection{ 0,0,0,0 };

		if (keys['A'])
		{
			_pLastMove = -1;
			moveDirection = DirectX::XMVectorAdd(moveDirection, DirectX::XMVectorSet(-1, 0, 0, 0));
		}
		if (keys['D'])
		{
			_pLastMove = 1;
			moveDirection = DirectX::XMVectorAdd(moveDirection, DirectX::XMVectorSet(1, 0, 0, 0));
		}

		position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(DirectX::XMVector3Normalize(moveDirection), _pSpeed * deltaTime));

		if (keys[' '])
		{
			const float x = cosf(_pLookAt) * static_cast<float>(_pLastMove) * _pJumpPow;
			const float y = sinf(_pLookAt) * _pJumpPow;
			potentialVelocity = DirectX::XMVectorSet(x, y, 0, 0);
		}
	}
	else
		potentialVelocity = DirectX::XMVectorAdd(_pVelocity, DirectX::XMVectorSet(0, -9.8f * deltaTime, 0, 0));

	position = DirectX::XMVectorAdd(DirectX::XMVectorScale(potentialVelocity, deltaTime), position);
	// parasoft-begin-suppress ALL "suppress all violations"
	auto temPos = _pPlayerObject->position();
	// parasoft-end-suppress ALL "suppress all violations"
	_pPlayerObject->setPosition(position);
	if(CheckCollisions(objects))
	{
		_pVelocity = DirectX::XMVectorSet(0, 0, 0, 0);
		_pOnGround = true;
		_pPlayerObject->setPosition(temPos);
	}
	else
	{
		_pOnGround = false;
		_pVelocity = potentialVelocity;
	}

	//Friction
	_pVelocity = DirectX::XMVectorScale(_pVelocity, 1 - _pFriction * deltaTime);

	const float x = cosf(_pLookAt) * static_cast<float>(_pLastMove);
	const float y = sinf(_pLookAt);
	const auto lookAtPos = DirectX::XMVectorAdd(position, DirectX::XMVectorSet(x * 2, y * 2, 0, 0));
	_pLookAtObject->setPosition(lookAtPos);

	const float rotationAngle = atan2f(y, x);
	_pLookAtObject->setRotation(DirectX::XMVectorSet(0, 0, rotationAngle, 0));
}

bool Player::CheckCollisions(const std::vector<std::shared_ptr<Object>>& objects) const
{
	for (const auto& object : objects)
	{
		if (object == _pPlayerObject)
			continue;

		if (!object->hasCollider())
			continue;

		if (_pPlayerObject->collider().intersects(object->collider()))
			return true;
	}

	return false;
}

