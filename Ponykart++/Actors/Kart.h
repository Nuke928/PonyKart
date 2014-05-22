#ifndef KART_H_INCLUDED
#define KART_H_INCLUDED

#include <vector>
#include <functional>
#include <BulletDynamics/Vehicle/btRaycastVehicle.h>
#include <OgreFrameListener.h>
#include "Actors/LThing.h"
#include "Actors/Wheels/Wheel.h"
#include "Core/RaceCountdown.h"

class btRaycastVehicle;
namespace PonykartParsers
{
	class ThingBlock;
	class ThingDefinition;
} // PonykartParsers

namespace Ponykart
{
namespace Physics{ class KartMotionState; }
namespace Players{ class Player; }
namespace Actors
{
class Driver;

//delegate void KartEvent(Kart kart); // TODO: Implement
using KartEvent = std::vector<std::function<void (Kart* kart)>>;

/// Base class for karts. -Z is forwards!
class Kart : public LThing
{
public:
	Kart(PonykartParsers::ThingBlock* block, PonykartParsers::ThingDefinition* def);
	void startDrifting(KartDriftState state);
	void stopDrifting(); ///< Stop drifting. This is run when we let go of the drift button.
	// Getters
	float getMaxSpeed() const;
	float getMaxReverseSpeed() const;
	float getMaxSpeedSquared() const;
	float getMaxReverseSpeedSquared() const;
	const Wheel* const getWheelFL() const;
	const Wheel* const getWheelFR() const;
	const Wheel* const getWheelBL() const;
	const Wheel* const getWheelBR() const;
	const btRaycastVehicle* const getVehicle() const;
	btRaycastVehicle* getVehicle();
	float getVehicleSpeed() const;
	bool isCompletelyDrifting() const; ///< Returns true if we're completely drifting - not starting, not stopping, but in between.
	bool isStartingDrifting() const; ///< Returns true if we're starting to drift
	bool isStoppingDrifting() const; ///< Returns true if we're stopping drifting
	bool isDriftingAtAll() const; ///< Returns true if we're drifting at all - starting, stopping, or in between.
	const btRaycastVehicle::btVehicleTuning* const getTuning() const;
	Ogre::Quaternion getActualOrientation() const;
	Ogre::Vector3 getActualPosition() const;
	const Ogre::SceneNode* const getLeftParticleNode() const;
	const Ogre::SceneNode* const getRightParticleNode() const;
	float getAcceleration() const;
	// Setters
	void setMaxSpeed(float speed);
	void setMaxReverseSpeed(float speed);
	void setTurnMultiplier(float multiplier);
	void setAcceleration(float newAcceleration);
protected:
//	MotionState initializationMotionState() override; // TODO: Find MotionState definition and implement
	virtual void postCreateBody(PonykartParsers::ThingDefinition* def) override; ///< After we create our RigidBody, we turn it into a vehicle
	virtual void postInitialiseComponents(PonykartParsers::ThingBlock* thingTemplate, PonykartParsers::ThingDefinition* def) override; /// Make some nodes for us to attach wheel particles to
private:
	void stopDrifting_WheelFunction(Wheel* w);
	void startDrifting_WheelFunction(Wheel* w);
	void finaliseBeforeSimulation(btDiscreteDynamicsWorld* world, const Ogre::FrameEvent& evt); ///< limit the wheels' speed!
	void onCountdown(Core::RaceCountdownState state); ///< Give it a second or two to get the wheels in the right positions before we can deactivate the karts when they're stopped
public:
	int ownerID; ///< A special ID number just for the karts. 0 is usually the player kart, but don't rely on this.
	const float defaultMaxSpeed;
	const float frontDriftAngle; ///< (RADIANS) The angle of the "front" wheels during drifting
	const float backDriftAngle; ///< (RADIANS) The angle of the "back" wheels during drifting
	const Ogre::Radian driftTransitionAngle; ///< (RADIANS) The angle the kart "jumps" through when starting drifting
	bool isInAir; ///< Should only be set by KartHandler
	KartDriftState driftState;
	Driver* driver;
	Players::Player* player;
	Physics::KartMotionState* kartMotionState;
	static KartEvent onStartDrifting, onDrifting, onStopDrifting, onFinishDrifting;
protected:
	Wheel* wheelFL;
	Wheel* wheelFR;
	Wheel* wheelBL;
	Wheel* wheelBR;
	btRaycastVehicle* _vehicle;
	btRaycastVehicle::btVehicleTuning* tuning;
	btVehicleRaycaster* raycaster;
	float turnMultiplier;
	float acceleration;
private:
	float maxSpeed;
	float maxSpeedSquared;
	float maxReverseSpeed;
	float maxReverseSpeedSquared;
	Ogre::SceneNode* leftParticleNode;
	Ogre::SceneNode* rightParticleNode;
	bool canDisableKarts = false;
};
} // Actors
} // Ponykart

#endif // KART_H_INCLUDED
