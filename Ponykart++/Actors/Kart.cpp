#include "pch.h"
#include "Actors/Kart.h"
#include "Actors/Wheels/WheelFactory.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelOgre.h"
#include "Misc/ogreExtensions.h"
#include "Misc/bulletExtensions.h"
#include "Physics/PhysicsMain.h"
#include "Physics/MotionStates/KartMotionState.h"
#include "Thing/ThingDefinition.h"
#include "Thing/Blocks/ThingBlock.h"

using namespace Ogre;
using namespace Extensions;
using namespace PonykartParsers;
using namespace Ponykart::Actors;
using namespace Ponykart::Actors::KartDriftStateExtensions;
using namespace Ponykart::Core;
using namespace Ponykart::LKernel;
using namespace Ponykart::Physics;

// Static members
KartEvent Kart::onStartDrifting, Kart::onDrifting, Kart::onStopDrifting, Kart::onFinishDrifting;

Kart::Kart(ThingBlock* block, ThingDefinition* def) : LThing (block, def),
	defaultMaxSpeed(def->getFloatProperty("maxspeed", 180)),
	frontDriftAngle(Degree(def->getFloatProperty("FrontDriftAngle", 46)).valueRadians()),
	backDriftAngle(Degree(def->getFloatProperty("BackDriftAngle", 55)).valueRadians()),
	driftTransitionAngle(Degree(def->getFloatProperty("DriftTransitionAngle", 40)))
{
	acceleration = 0;

	maxSpeed = defaultMaxSpeed;
	maxReverseSpeed = def->getFloatProperty("maxreversespeed", 4);

	maxSpeedSquared = maxSpeed * maxSpeed;
	maxReverseSpeedSquared = maxReverseSpeed * maxReverseSpeed;

	isInAir = false;

	// Call our overriden LThing methods, LThing::LThing() can't possible call them.
	postInitialiseComponents(block, def);
	postCreateBody(def);
}

float Kart::getMaxSpeed() const
{
	return maxSpeed;
}

void Kart::setMaxSpeed(float speed)
{
	maxSpeed = speed;
	maxSpeedSquared = speed*speed;
}

float Kart::getMaxReverseSpeed() const
{
	return maxReverseSpeed;
}

void Kart::setMaxReverseSpeed(float speed)
{
	maxReverseSpeed = speed;
	maxReverseSpeedSquared = speed*speed;
}

const btRaycastVehicle* const Kart::getVehicle() const
{
	return _vehicle;
}

btRaycastVehicle* Kart::getVehicle()
{
	return _vehicle;
}

const btRaycastVehicle::btVehicleTuning* const Kart::getTuning() const
{
	return tuning;
}

Ogre::Quaternion Kart::getActualOrientation() const
{
	return toOgreQuaternion(kartMotionState->actualOrientation);
}

float Kart::getVehicleSpeed() const
{
	return _vehicle->getCurrentSpeedKmHour();
}

bool Kart::isDriftingAtAll() const
{
	return KartDriftStateExtensions::isDriftingAtAll(driftState);
}

Ogre::Vector3 Kart::getActualPosition() const
{
	return toOgreVector3(kartMotionState->actualPosition);
}

void Kart::setTurnMultiplier(float multiplier)
{
	turnMultiplier = multiplier;

	wheelFL->turnMultiplier = multiplier;
	wheelFR->turnMultiplier = multiplier;
	wheelBL->turnMultiplier = multiplier;
	wheelBR->turnMultiplier = multiplier;
}

float Kart::getAcceleration() const
{
	return acceleration;
}

void Kart::setAcceleration(float newAcceleration)
{
	acceleration = newAcceleration;
	if (acceleration != 0.f)
		body->activate();

	wheelFL->accelerateMultiplier = newAcceleration;
	wheelFL->isBrakeOn = false;
	wheelFR->accelerateMultiplier = newAcceleration;
	wheelFR->isBrakeOn = false;
	wheelBL->accelerateMultiplier = newAcceleration;
	wheelBL->isBrakeOn = false;
	wheelBR->accelerateMultiplier = newAcceleration;
	wheelBR->isBrakeOn = false;
}

void Kart::stopDrifting()
{
	// "upgrade" our events
	if (driftState == KartDriftState::FullLeft || driftState == KartDriftState::StartLeft)
		driftState = KartDriftState::StopLeft;
	else if (driftState == KartDriftState::FullRight || driftState == KartDriftState::StartRight)
		driftState = KartDriftState::StopRight;

	// make the wheels back to normal
	stopDrifting_WheelFunction(wheelFL);
	stopDrifting_WheelFunction(wheelFR);
	stopDrifting_WheelFunction(wheelBL);
	stopDrifting_WheelFunction(wheelBR);

	// eeeeeveeeeeent
	if (onStopDrifting.size())
		for (auto& fun : onStopDrifting)
			fun(this);
}

void Kart::stopDrifting_WheelFunction(Wheel* w)
{
	w->driftState = WheelDriftState::None;
	w->idealSteerAngle = 0.f;

	if (w->getId() == WheelID::FrontRight || w->getId() == WheelID::FrontLeft)
		_vehicle->getWheelInfo(w->intWheelID).m_bIsFrontWheel = true;
	else
		_vehicle->getWheelInfo(w->intWheelID).m_bIsFrontWheel = false;
}

/// Start drifting in a certain direction
/// @param state This must be either StartDriftLeft or StartDriftRight
void Kart::startDrifting(KartDriftState state)
{
	// first check to make sure we weren't passed an incorrect argument
	if (!isStartDrift(state))
		throw std::string("You must pass either StartDriftLeft or StartDriftRight!");

	if (_vehicle->getCurrentSpeedKmHour() < 20.f || KartDriftStateExtensions::isDriftingAtAll(driftState))
		return;

	// update our state
	driftState = state;

	startDrifting_WheelFunction(wheelFL);
	startDrifting_WheelFunction(wheelFR);
	startDrifting_WheelFunction(wheelBL);
	startDrifting_WheelFunction(wheelBR);

	if (onStartDrifting.size())
		for (auto& fun : onStartDrifting)
			fun(this);
}

/// Put this in a separate function so we can edit it at runtime, since VS doesn't like us trying to edit anonymous functions
void Kart::startDrifting_WheelFunction(Wheel* w)
{
	// left
	if (driftState == KartDriftState::StartLeft) 
	{
		w->driftState = WheelDriftState::Left;

		// change the back wheels' angles
		if (w->getId() == WheelID::FrontRight || w->getId() == WheelID::BackRight) 
		{
			w->idealSteerAngle = backDriftAngle;
			_vehicle->getWheelInfo(w->intWheelID).m_bIsFrontWheel = false;
		}
		// change the front wheels' angles
		else 
		{
			w->idealSteerAngle = frontDriftAngle;
			_vehicle->getWheelInfo(w->intWheelID).m_bIsFrontWheel = true;
		}
	}
	// right
	else if (driftState == KartDriftState::StartRight) 
	{
		w->driftState = WheelDriftState::Right;

		// change the back wheels' angles
		if (w->getId() == WheelID::FrontLeft || w->getId() == WheelID::BackLeft) 
		{
			w->idealSteerAngle = -backDriftAngle;
			_vehicle->getWheelInfo(w->intWheelID).m_bIsFrontWheel = false;
		}
		// change the front wheels' angles
		else 
		{
			w->idealSteerAngle = -frontDriftAngle;
			_vehicle->getWheelInfo(w->intWheelID).m_bIsFrontWheel = true;
		}
	}
}

void Kart::postCreateBody(ThingDefinition* def)
{
	kartMotionState = (KartMotionState*)motionState;

	body->setCcdMotionThreshold(0.001f);
	body->setCcdSweptSphereRadius(0.04f);

	raycaster = new btDefaultVehicleRaycaster(getG<PhysicsMain>()->getWorld());
	tuning = new btRaycastVehicle::btVehicleTuning();
	_vehicle = new btRaycastVehicle(*tuning, body, raycaster);
	_vehicle->setCoordinateSystem(0, 1, 2); // I have no idea what this does... I'm assuming something to do with a rotation matrix?

	getG<PhysicsMain>()->getWorld()->addAction(_vehicle);

	WheelFactory* wheelFac = getG<WheelFactory>();
	std::string frontWheelName = def->getStringProperty("FrontWheel", "");
	std::string backWheelName = def->getStringProperty("BackWheel", "");
	wheelFL = wheelFac->createWheel(frontWheelName, WheelID::FrontLeft, this, def->getVectorProperty("FrontLeftWheelPosition", {}), def->getStringProperty("FrontLeftWheelMesh", ""));
	wheelFR = wheelFac->createWheel(frontWheelName, WheelID::FrontRight, this, def->getVectorProperty("FrontRightWheelPosition", {}), def->getStringProperty("FrontRightWheelMesh", ""));
	wheelBL = wheelFac->createWheel(backWheelName, WheelID::BackLeft, this, def->getVectorProperty("BackLeftWheelPosition", {}), def->getStringProperty("BackLeftWheelMesh", ""));
	wheelBR = wheelFac->createWheel(backWheelName, WheelID::BackRight, this, def->getVectorProperty("BackRightWheelPosition", {}), def->getStringProperty("BackRightWheelMesh", ""));

	leftParticleNode->setPosition(leftParticleNode->getPosition()-Vector3(0, wheelBL->defaultRadius * 0.7f, 0));
	rightParticleNode->setPosition(rightParticleNode->getPosition()-Vector3(0, wheelBR->defaultRadius * 0.7f, 0));

	body->setLinearVelocity(btVector3(0, 1, 0));

	PhysicsMain::finaliseBeforeSimulation.push_back(std::bind(&Kart::finaliseBeforeSimulation,this,std::placeholders::_1,std::placeholders::_2));
	getG<RaceCountdown>()->onCountdown.push_back({ id, std::bind(&Kart::onCountdown, this, std::placeholders::_1) });
}

void Kart::finaliseBeforeSimulation(btDiscreteDynamicsWorld* world, const FrameEvent& evt)
{
	float currentSpeed = _vehicle->getCurrentSpeedKmHour();
	bool isDriftingAtAll = ::isDriftingAtAll(driftState);

	if (!isInAir) 
	{
		// going forwards
		// using 20 because we don't need to check the kart's linear velocity if it's going really slowly
		if ((currentSpeed > 20.f && !isDriftingAtAll)
			|| currentSpeed < -20.f && isDriftingAtAll
			|| currentSpeed > 20.f && isDriftingAtAll)
		{
			// check its velocity against the max velocity (both are squared to avoid unnecessary square roots)
			if (toOgreVector3(body->getLinearVelocity()).squaredLength() > maxSpeedSquared) 
			{
				Vector3 vec = toOgreVector3(body->getLinearVelocity());
				vec.normalise();
				vec *= maxSpeed;
				body->setLinearVelocity(toBtVector3(vec));
			}
		}
		// going in reverse, so we want to limit the speed even more
		else if (currentSpeed < -20.f && !isDriftingAtAll) 
		{
			if (toOgreVector3(body->getLinearVelocity()).squaredLength() > maxReverseSpeedSquared) 
			{
				Vector3 vec = toOgreVector3(body->getLinearVelocity());
				vec.normalise();
				vec *= maxReverseSpeed;
				body->setLinearVelocity(toBtVector3(vec));
			}
		}
		else if (currentSpeed < 4.f && currentSpeed > -4.f)
		{
			if (canDisableKarts && acceleration == 0.f)
				body->forceActivationState(WANTS_DEACTIVATION);
		}
	}
}

void Kart::onCountdown(RaceCountdownState state)
{
	if (state == RaceCountdownState::Two) 
	{
		canDisableKarts = true; 

		auto a = std::bind(&Kart::onCountdown, this, std::placeholders::_1);
		auto& RCOnCountdown = getG<RaceCountdown>()->onCountdown;

		auto it = find_if(begin(RCOnCountdown), end(RCOnCountdown),
			[&](std::pair<int, std::function<void(RaceCountdownState)>> a){return a.first == id; });
		if (it != end(RCOnCountdown))
			RCOnCountdown.erase(it);
	}
}

void Kart::postInitialiseComponents(PonykartParsers::ThingBlock* thingTemplate, ThingDefinition* def)
{
	Vector3 frontleft = def->getVectorProperty("FrontLeftWheelPosition", {});
	Vector3 frontright = def->getVectorProperty("FrontRightWheelPosition", {});
	Vector3 backleft = def->getVectorProperty("BackLeftWheelPosition", {});
	Vector3 backright = def->getVectorProperty("BackRightWheelPosition", {});
	leftParticleNode = rootNode->createChildSceneNode(frontleft.midPoint(backleft));
	rightParticleNode = rootNode->createChildSceneNode(frontright.midPoint(backright));
}
