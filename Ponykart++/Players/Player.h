#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <string>
#include "Actors/Driver.h"
#include "Actors/Kart.h"


namespace Ponykart
{
namespace Levels{ class LevelChangedEventArgs; }
namespace Players
{
// Abstract class for players - each player controls a kart, and abstracting away the player will help when it comes to things like AI and/or networking
class Player // TODO: Implement shortcuts and key events. Finish implementing the rest.
{
public:
	Player(Levels::LevelChangedEventArgs* eventArgs, int Id, bool IsComputerControlled);
	virtual void detach();
	// Getters
	const Actors::Kart* const getKart() const;
	const Actors::Driver* const getDriver() const;
	const btRigidBody* const getBody() const; ///< Gets the kart's Body
	int getId() const;
	const std::string& getCharacter();
	bool getIsComputerControlled();
	bool getIsLocal();
	const Ogre::SceneNode* const getNode(); // Gets the kart's SceneNode
	//const RigidBody* const getRigidBody(); // Gets the kart's Body // TODO: Implement with OgreBullet
	const Ogre::Quaternion* const getOrientation(); // Gets the kart's SceneNode's orientation
	// Gets the kart's Node's position. No setter because it's automatically changed to whatever the position of its body is
	// Use the Bullet RigidBody if you want to change the kart's position!
	const Ogre::Vector3 getNodePosition() const;
protected:
	Player(); // Set some default values
	virtual void onSteeringChanged (float value);
	virtual void onAccelerateChanged (float value);
	virtual void onBrakeChanged (float value);
	virtual void onStartDrift() {}
	virtual void onStopDrift() {}
	virtual void useItem()=0; // Uses an item
public:
	bool isControlEnabled; // Can the player control his kart?
protected:
	Actors::Kart* kart; // The kart that this player is driving
	Actors::Driver* driver; // The driver in the kart
	int id; // id number. Same thing that's used as the array index in PlayerManager.
	std::string character;
	bool hasItem;
	std::string heldItem;

	float steeringAxis;
	float accelAxis;
	float brakeAxis;

private:
	bool isComputerControlled;
	bool isLocal;

};
} // Players
} // Ponykart

#endif // PLAYER_H_INCLUDED
