#include "pch.h"
#include <string>
#include <OgreCamera.h>
#include <OgreRoot.h>
#include <OgreSceneNode.h>
#include "Core/Cameras/CameraManager.h"
#include "Core/Cameras/LCamera.h"
#include "Kernel/LKernel.h"

using namespace Ponykart::Core;
using namespace Ponykart::LKernel;

LCamera::LCamera(const std::string& Name)
 : name(Name)
{
}

const Ogre::Camera* const LCamera::getCamera() const
{
	return camera;
}

const Ogre::SceneNode* const LCamera::getCameraNode() const
{
	return cameraNode;
}
std::string LCamera::getName() const
{
	return name;
}

bool LCamera::updateCamera(const Ogre::FrameEvent& evt)
{
	return true;
}

/// \todo Implement this
void LCamera::onSwitchToActive(LCamera *oldCamera)
{
	// TODO
	isActive = true;
}

/// \todo Implement this
void LCamera::onSwitchToInactive(LCamera *oldCamera)
{
	// TODO
	isActive = false;
}

void LCamera::registerCam()
{
	getG<CameraManager>()->registerCamera(this);
}

void LCamera::makeActiveCamera()
{
	getG<CameraManager>()->switchCurrentCamera(this);
}
