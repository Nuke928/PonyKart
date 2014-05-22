#include "pch.h"
#include <OgreSceneNode.h>
#include "Core/Cameras/PlayerCamera.h"
#include "Core/Settings.h"
#include "Kernel/LKernel.h"
#include "Players/PlayerManager.h"
#include "Players/Player.h"
#include "Physics/PhysicsMain.h"

using namespace Ponykart::Core;
using namespace Ponykart::LKernel;
using namespace Ponykart::Players;
using namespace Ponykart::Physics;
using namespace Ogre;
using namespace std;

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

	cameraNode->setAutoTracking(true, targetNode);
	cameraNode->setFixedYawAxis(true);
	cameraNode->attachObject(camera);

	// create the fixed nodes that are attached to the kart
	followKart = getG<PlayerManager>()->getMainPlayer()->getKart();
	kartCamNode = followKart->getRootNode()->createChildSceneNode(name + "_KartCameraNode", Vector3(0, Settings::CameraNodeYOffset, Settings::CameraNodeZOffset));
	kartTargetNode = followKart->getRootNode()->createChildSceneNode(name + "_KartCameraTargetNode", Vector3(0, Settings::CameraTargetYOffset, 0));

	cameraNode->setPosition(kartCamNode->_getDerivedPosition());
	targetNode->setPosition(kartTargetNode->_getDerivedPosition());

	// initialise some stuff for the ray casting
	rayLength = (cameraNode->getPosition() - targetNode->getPosition()).length();
	world = getG<PhysicsMain>()->getWorld();
}
