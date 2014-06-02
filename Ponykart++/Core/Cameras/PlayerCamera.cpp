#include "pch.h"
#include <OgreSceneNode.h>
#include <cmath>
#include "Core/Cameras/PlayerCamera.h"
#include "Core/Settings.h"
#include "Kernel/LKernel.h"
#include "Misc/bulletExtensions.h"
#include "Misc/ogreExtensions.h"
#include "Players/PlayerManager.h"
#include "Players/Player.h"
#include "Physics/PhysicsMain.h"

using namespace Extensions;
using namespace Ponykart::Core;
using namespace Ponykart::LKernel;
using namespace Ponykart::Players;
using namespace Ponykart::Physics;
using namespace Ogre;
using namespace std;

const float PlayerCamera::_cameraTightness = Settings::CameraTightness;
const float PlayerCamera::_cameraTargetYOffset = Settings::CameraTargetYOffset;

PlayerCamera::PlayerCamera(const std::string& name)
: LCamera(name)
{
	SceneManager* sceneMgr = getG<SceneManager>();

	// make our camera and set some properties
	camera = sceneMgr->createCamera(name);
	camera->setNearClipDistance(0.1f);
	camera->setFarClipDistance(300.f);
	camera->setAutoAspectRatio(true);

	// create the nodes we're going to interpolate
	cameraNode = sceneMgr->getRootSceneNode()->createChildSceneNode(name+"_PlayerCameraNode", Vector3(0, Settings::CameraNodeYOffset, Settings::CameraNodeZOffset));
    targetNode = sceneMgr->getRootSceneNode()->createChildSceneNode(name+"_PlayerCameraTargetNode", Vector3(0, Settings::CameraTargetYOffset, 0));

	//cameraNode->setAutoTracking(true, targetNode);
	cameraNode->setFixedYawAxis(true);
    cameraNode->attachObject(camera);

	// create the fixed nodes that are attached to the kart
	followKart = getG<PlayerManager>()->getMainPlayer()->getKart();
	kartCamNode = followKart->getRootNode()->createChildSceneNode(name + "_KartCameraNode", Vector3(0, Settings::CameraNodeYOffset, Settings::CameraNodeZOffset));
    kartTargetNode = followKart->getRootNode()->createChildSceneNode(name + "_KartCameraTargetNode", Vector3(0, Settings::CameraTargetYOffset, 0));

	cameraNode->setPosition(kartCamNode->_getDerivedPosition());
    //targetNode->setPosition(kartTargetNode->_getDerivedPosition());

	// initialise some stuff for the ray casting
    rayLength = (cameraNode->getPosition() - targetNode->getPosition()).length();
	world = getG<PhysicsMain>()->getWorld();

	camera->setAutoTracking(true, followKart->getRootNode(), Vector3(0, 0.4f, 0));
    //kartCamNode->attachObject(camera);
}

bool PlayerCamera::updateCamera(const Ogre::FrameEvent& evt)
{
	//cameraNode->setPosition(kartCamNode->_getDerivedPosition());
    //return true;


	// TODO: BUG: FIXME: Still somewhat jittery.
	Vector3 camDisplacement, targetDisplacement,
		derivedCam = kartCamNode->_getDerivedPosition(),
		derivedTarget = kartTargetNode->_getDerivedPosition();

	camDisplacement = derivedCam - cameraNode->getPosition();
	targetDisplacement = derivedTarget - targetNode->getPosition();

	cameraNode->translate(camDisplacement * _cameraTightness * evt.timeSinceLastFrame);
	targetNode->translate(targetDisplacement * _cameraTightness * evt.timeSinceLastFrame);
	return true;

}

btDynamicsWorld::ClosestRayResultCallback PlayerCamera::castRay(Ogre::Vector3 derivedCam, Ogre::Vector3 derivedTarget)
{
	Vector3 from = derivedTarget;
	Vector3 axis = (from - derivedCam);
	axis.normalise();
	Vector3 to = from - (axis * rayLength); //CameraNode.Position;

	btVector3 btFrom = toBtVector3(from), btTo = toBtVector3(to);
	auto callback = btDynamicsWorld::ClosestRayResultCallback(btFrom, btTo);
	callback.m_collisionFilterMask = 2 | 8 | 64; // Environment, road, invisible walls

	world->rayTest(btFrom, btTo, callback);

	return callback;
}
