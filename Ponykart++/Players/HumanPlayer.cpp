#include "pch.h"
#include "Actors/Wheels/DriftState.h"
#include "Input/KeyBindingManager.h"
#include "Items/ItemManager.h"
#include "Kernel/LKernel.h"
#include "Players/HumanPlayer.h"
#include "UI/GameUIManager.h"

using namespace std;
using namespace Ponykart;
using namespace Ponykart::Actors;
using namespace Ponykart::Input;
using namespace Ponykart::Items;
using namespace Ponykart::Levels;
using namespace Ponykart::LKernel;
using namespace Ponykart::Players;

HumanPlayer::HumanPlayer(LevelChangedEventArgs* eventArgs, int id)
: Player(eventArgs, id, false)
{
	// hook up to input events
	bindings = LKernel::get<KeyBindingManager>();

	LKernel::getG<UI::GameUIManager>()->setItemLevel(0);

	pressConnection = bindings->pressEvent.connect(bind(&HumanPlayer::pressHandler, this, std::placeholders::_1, std::placeholders::_2));
	releaseConnection = bindings->releaseEvent.connect(bind(&HumanPlayer::releaseHandler, this, std::placeholders::_1, std::placeholders::_2));
	axisMoveConnection = bindings->axisMoveEvent.connect(bind(&HumanPlayer::axisMoveHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void HumanPlayer::pressHandler (int playerID, Input::GameInputID inputID)
{
	if (playerID != id)
		return;

	switch (inputID) {
	case GameInputID::TurnLeft:
		onSteeringChanged(bindings->pollKey(id, GameInputID::TurnRight) ? 0 : -1.f);
		break;
	case GameInputID::TurnRight:
		onSteeringChanged(bindings->pollKey(id, GameInputID::TurnLeft) ? 0 : 1.f);
		break;
	case GameInputID::Accelerate:
		onAccelerateChanged(1.f);
		break;
	case GameInputID::Reverse:
		onBrakeChanged(1.f);
		break;
	case GameInputID::Drift:
		onStartDrift();
		break;
	case GameInputID::Item:
		useItem();
		break;
	}
}
void HumanPlayer::releaseHandler (int playerID, Input::GameInputID inputID)
{
	if (playerID != id)
		return;

	switch (inputID) {
	case GameInputID::TurnLeft:
		onSteeringChanged(bindings->pollKey(id, GameInputID::TurnRight) ? 1.f : bindings->pollAxis(id, GameInputID::SteeringAxis));
		break;
	case GameInputID::TurnRight:
		onSteeringChanged(bindings->pollKey(id, GameInputID::TurnLeft) ? -1.f : bindings->pollAxis(id, GameInputID::SteeringAxis));
		break;
	case GameInputID::Accelerate:
		onAccelerateChanged(bindings->pollAxis(id, GameInputID::AccelerateAxis));
		break;
	case GameInputID::Reverse:
        onAccelerateChanged(bindings->pollAxis(id, GameInputID::BrakeAxis));
		break;
	case GameInputID::Drift:
		onStopDrift();
		break;
	}
}
void HumanPlayer::axisMoveHandler (int playerID, Input::GameInputID inputID, float value)
{
	if (playerID != id)
		return;

	switch (inputID) {
	case GameInputID::SteeringAxis:
		if (!bindings->pollKey(id, GameInputID::TurnLeft) && !bindings->pollKey(id, GameInputID::TurnRight))
			onSteeringChanged(value);
		break;
	case GameInputID::AccelerateAxis:
		if (!bindings->pollKey(id, GameInputID::Accelerate))
			onAccelerateChanged(value);
		break;
	case GameInputID::BrakeAxis:
		if (!bindings->pollKey(id, GameInputID::Reverse))
			onBrakeChanged(value);
		break;
	}
}


void HumanPlayer::onSteeringChanged (float value)
{
	Player::onSteeringChanged(value);

	if (isControlEnabled) {
#ifdef DRIFTING_ENABLED
		if (kart->driftState == KartDriftState::WantsDriftingButNotTurning && steeringAxis != 0.f)
			kart->startDrifting(steeringAxis < 0.f ? KartDriftState::StartLeft : KartDriftState::StartRight);
		// normal steering
		else
#endif
		{
			kart->setTurnMultiplier(steeringAxis);
		}
	}
}

void HumanPlayer::onAccelerateChanged (float value)
{
    Player::onAccelerateChanged(value);

	if (isControlEnabled)
		kart->setAcceleration(accelAxis - brakeAxis);
}

void HumanPlayer::onBrakeChanged (float value)
{
	Player::onBrakeChanged(value);

	if (isControlEnabled)
		kart->setAcceleration(accelAxis - brakeAxis);
}

void HumanPlayer::onStartDrift()
{
	Player::onStartDrift();

#ifdef DRIFTING_ENABLED
	if (isControlEnabled)
	{
		// if left is pressed and right isn't, start drifting left
		if (steeringAxis < 0.f)
			kart->startDrifting(KartDriftState::StartRight);
		// otherwise if right is pressed and left isn't, start drifting right
		else if (steeringAxis > 0.f)
			kart->startDrifting(KartDriftState::StartLeft);
		// otherwise it wants to drift but we don't have a direction yet
		else if (kart->getVehicleSpeed() > 20.f)
			kart->driftState = KartDriftState::WantsDriftingButNotTurning;
	}
#endif
}
/// Cancel the drift
void HumanPlayer::onStopDrift() {
	Player::onStopDrift();

#ifdef DRIFTING_ENABLED
	if (isControlEnabled)
	{
		// if we were drifting left
		if (kart->driftState == KartDriftState::FullLeft || kart->driftState == KartDriftState::StartLeft)
			kart->stopDrifting();
		// if we were drifting right
		else if (kart->driftState == KartDriftState::FullRight || kart->driftState == KartDriftState::StartRight)
			kart->stopDrifting();
		// if we had the drift button down but weren't actually drifting
		else if (kart->driftState == KartDriftState::WantsDriftingButNotTurning)
			kart->driftState = KartDriftState::None;
	}
#endif
}

void HumanPlayer::useItem ()
{
	if (hasItem)
	{
		LKernel::getG<UI::GameUIManager>()->setItemLevel(0);
		LKernel::getG<UI::GameUIManager>()->setItemImage("none");
		LKernel::getG<ItemManager>()->spawnItem(this, heldItem);
	}
	else
	{
		//Ogre::Vector3 pos = kart->getActualPosition();
		//pos.y += 5;
		//LKernel::getG<ItemManager>()->requestBox(pos);
	}
	hasItem = false;
}






/*
void HumanPlayer::onStartAccelerate()
{
	Player::onStartAccelerate();

	if (isControlEnabled)
	{
		// if we have both forward and reverse pressed at the same time, do nothing
		if (bindings->isKeyPressed(LKey::Reverse))
			kart->setAcceleration(0.f);
		// otherwise go forwards as normal
		else
			kart->setAcceleration(1.f);
	}
}

void HumanPlayer::onStopTurnRight()
{
	Player::onStopTurnRight();

	if (isControlEnabled) 
	{
		// if left is still pressed, turn left
		if (bindings->isKeyPressed(LKey::TurnLeft))
			kart->setTurnMultiplier(1.f);
		// otherwise go straight
		else
			kart->setTurnMultiplier(0.f);
	}
}

void HumanPlayer::onStopTurnLeft()
{
	Player::onStopTurnLeft();

	if (isControlEnabled) 
	{
		// if right is still pressed, turn right
		if (bindings->isKeyPressed(LKey::TurnRight))
			kart->setTurnMultiplier(-1.f);
		// otherwise go straight
		else
			kart->setTurnMultiplier(0.f);
	}
}

void HumanPlayer::onStopReverse() 
{
	Player::onStopReverse();

	if (isControlEnabled) 
	{
		// if forward is still held down, then we start going forwards
		if (bindings->isKeyPressed(LKey::Accelerate))
			kart->setAcceleration(1.f);
		// otherwise we just stop accelerating
		else
			kart->setAcceleration(0.f);
	}
}

void HumanPlayer::onStopAccelerate() 
{
	Player::onStopAccelerate();

	if (isControlEnabled) 
	{
		// if reverse is still held down, then we start reversing
		if (bindings->isKeyPressed(LKey::Reverse))
			kart->setAcceleration(-1.f);
		// otherwise we just stop accelerating
		else
			kart->setAcceleration(0.f);
	}
}

void HumanPlayer::onStartTurnRight() 
{
	Player::onStartTurnRight();

	if (isControlEnabled) 
	{
#ifdef DRIFTING_ENABLED
		if (kart->driftState == KartDriftState::WantsDriftingButNotTurning)
			kart->startDrifting(KartDriftState::StartLeft);
		// normal steering
		else
#endif
		{
			// if both turns are pressed, we go straight
			if (bindings->isKeyPressed(LKey::TurnLeft))
				kart->setTurnMultiplier(0.f);
			// otherwise go right
			else
				kart->setTurnMultiplier(-1.f);
		}
	}
}

void HumanPlayer::onStartTurnLeft() {
	Player::onStartTurnLeft();

	if (isControlEnabled) 
	{
#ifdef DRIFTING_ENABLED
		// if we're waiting to drift
		if (kart->driftState == KartDriftState::WantsDriftingButNotTurning)
			kart->startDrifting(KartDriftState::StartRight);
		// normal steering
		else
#endif
		{
			// if both turns are pressed, we go straight
			if (bindings->isKeyPressed(LKey::TurnRight))
				kart->setTurnMultiplier(0.f);
			// otherwise go left
			else
				kart->setTurnMultiplier(1.f);
		}
	}
}

void HumanPlayer::onStartReverse() 
{
	Player::onStartReverse();

	if (isControlEnabled) 
	{
		// if we have both forward and reverse pressed at the same time, do nothing
		if (bindings->isKeyPressed(LKey::Accelerate))
			kart->setAcceleration(0.f);
		// otherwise go forwards as normal
		else
			kart->setAcceleration(-1.f);
	}
}
*/
