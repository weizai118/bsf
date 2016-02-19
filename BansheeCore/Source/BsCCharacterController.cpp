//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCCharacterController.h"
#include "BsSceneObject.h"
#include "BsCCharacterControllerRTTI.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	CCharacterController::CCharacterController(const HSceneObject& parent)
		: Component(parent)
	{
		setName("CharacterController");

		mNotifyFlags = TCF_Transform;
	}

	CharacterCollisionFlags CCharacterController::move(const Vector3& displacement)
	{
		CharacterCollisionFlags output;

		if (mInternal == nullptr)
			return output;

		output = mInternal->move(displacement);
		updatePositionFromController();
	}

	Vector3 CCharacterController::getFootPosition() const
	{
		if (mInternal == nullptr)
			return Vector3::ZERO;

		return mInternal->getFootPosition();
	}

	void CCharacterController::setFootPosition(const Vector3& position)
	{
		if (mInternal == nullptr)
			return;

		mInternal->setFootPosition(position);
		updatePositionFromController();
	}

	void CCharacterController::setRadius(float radius)
	{
		mDesc.radius = radius;

		if (mInternal != nullptr)
			updateDimensions();
	}

	void CCharacterController::setHeight(float height)
	{
		mDesc.height = height;

		if (mInternal != nullptr)
			updateDimensions();
	}

	void CCharacterController::setUp(const Vector3& up)
	{
		mDesc.up = up;

		if (mInternal != nullptr)
			mInternal->setUp(up);
	}

	void CCharacterController::setClimbingMode(CharacterClimbingMode mode)
	{
		mDesc.climbingMode = mode;

		if (mInternal != nullptr)
			mInternal->setClimbingMode(mode);
	}

	void CCharacterController::setNonWalkableMode(CharacterNonWalkableMode mode)
	{
		mDesc.nonWalkableMode = mode;

		if (mInternal != nullptr)
			mInternal->setNonWalkableMode(mode);
	}

	void CCharacterController::setMinMoveDistance(float value)
	{
		mDesc.minMoveDistance = value;

		if (mInternal != nullptr)
			mInternal->setMinMoveDistance(value);
	}

	void CCharacterController::setContactOffset(float value)
	{
		mDesc.contactOffset = value;

		if (mInternal != nullptr)
			mInternal->setContactOffset(value);
	}

	void CCharacterController::setStepOffset(float value)
	{
		mDesc.stepOffset = value;

		if (mInternal != nullptr)
			mInternal->setStepOffset(value);
	}

	void CCharacterController::setSlopeLimit(Radian value)
	{
		mDesc.slopeLimit = value;

		if (mInternal != nullptr)
			mInternal->setSlopeLimit(value);
	}

	void CCharacterController::setLayer(UINT64 layer)
	{
		mLayer = layer;

		if (mInternal != nullptr)
			mInternal->setLayer(layer);
	}

	void CCharacterController::onInitialized()
	{

	}

	void CCharacterController::onDestroyed()
	{
		// This should release the last reference and destroy the internal controller
		mInternal = nullptr;
	}

	void CCharacterController::onDisabled()
	{
		// This should release the last reference and destroy the internal controller
		mInternal = nullptr;
	}

	void CCharacterController::onEnabled()
	{
		mDesc.position = SO()->getWorldPosition();
		mInternal = CharacterController::create(mDesc);

		mInternal->onColliderHit.connect(std::bind(&CCharacterController::triggerOnColliderHit, this, _1));
		mInternal->onControllerHit.connect(std::bind(&CCharacterController::triggerOnControllerHit, this, _1));

		mInternal->setLayer(mLayer);
		updateDimensions();
	}

	void CCharacterController::onTransformChanged(TransformChangedFlags flags)
	{
		if (!SO()->getActive() || mInternal == nullptr)
			return;

		mInternal->setPosition(SO()->getWorldPosition());
	}

	void CCharacterController::updatePositionFromController()
	{
		mNotifyFlags = (TransformChangedFlags)0;
		SO()->setWorldPosition(mInternal->getPosition());
		mNotifyFlags = TCF_Transform;
	}

	void CCharacterController::updateDimensions()
	{
		Vector3 scale = SO()->getWorldScale();
		float height = mDesc.height * Math::abs(scale.y);
		float radius = mDesc.radius * Math::abs(std::max(scale.x, scale.z));

		mInternal->setHeight(height);
		mInternal->setRadius(radius);
	}

	void CCharacterController::triggerOnColliderHit(const ControllerColliderCollision& value)
	{
		onColliderHit(value);
	}

	void CCharacterController::triggerOnControllerHit(const ControllerControllerCollision& value)
	{
		onControllerHit(value);
	}

	RTTITypeBase* CCharacterController::getRTTIStatic()
	{
		return CCharacterControllerRTTI::instance();
	}

	RTTITypeBase* CCharacterController::getRTTI() const
	{
		return CCharacterController::getRTTIStatic();
	}
}