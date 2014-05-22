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
using namespace Ogre;

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

void LCamera::onSwitchToActive(LCamera *oldCamera)
{
	isActive = true;
	getG<Root>()->addFrameListener(this);
}

void LCamera::onSwitchToInactive(LCamera *oldCamera)
{
	isActive = false;
	getG<Root>()->removeFrameListener(this);
}

void LCamera::registerCam()
{
	getG<CameraManager>()->registerCamera(this);
}

void LCamera::makeActiveCamera()
{
	getG<CameraManager>()->switchCurrentCamera(this);
}

bool LCamera::frameStarted(const FrameEvent& evt)
{
	return updateCamera(evt);
}
