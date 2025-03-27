#pragma once
#include <bitset>
#include "Object.h"

class Player final
{
	std::shared_ptr<Object> _pPlayerObject = nullptr;
	std::shared_ptr<Object> _pLookAtObject = nullptr;
	DirectX::XMVECTOR _pVelocity = DirectX::XMVectorSet(0, 0, 0, 0);
	float _pFriction = 0.2f, _pSpeed = 12.5f, _pJumpPow = 11.0f, _pLookAt = 0;
	int _pLastMove = 1;
	bool _pOnGround = false;

	bool CheckCollisions(const std::vector<std::shared_ptr<Object>>& objects) const;
public:
	Player() = delete;
	Player(const std::shared_ptr<Object>& playerObject, const std::shared_ptr<Object>& lookAtObject) : _pPlayerObject(playerObject), _pLookAtObject(lookAtObject) {}
	Player(const Player&) = delete;

	void update(const float deltaTime, const std::bitset<256>& keys, const std::vector<std::shared_ptr<Object>>& objects);

	void setFriction(const float friction) { _pFriction = friction; }
	void setSpeed(const float speed) { _pSpeed = speed; }
	void setJumpPower(const float jumpPow) { _pJumpPow = jumpPow; }

	float getFriction() const { return _pFriction; }
	float getSpeed() const { return _pSpeed; }
	float getJumpPower() const { return _pJumpPow; }
	float getLookAt() const { return _pLookAt; }
	float getLastMoveDir() const { return _pLastMove; }
	const DirectX::XMVECTOR& getVelocity() const { return _pVelocity; }
	Object& getPlayerObject() const { return *_pPlayerObject; }
	
};

