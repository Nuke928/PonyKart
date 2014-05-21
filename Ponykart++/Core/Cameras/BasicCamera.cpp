#include "pch.h"
#include <OgreSceneManager.h>
#include "Core/Cameras/BasicCamera.h"
#include "Kernel/LKernel.h"

using namespace Ponykart::Core;
using namespace Ponykart::LKernel;
using namespace Ogre;

BasicCamera::BasicCamera(const std::string& name)
: LCamera(name)
{
	SceneManager* sceneMgr = getG<SceneManager>();

	camera = sceneMgr->createCamera(name);
	camera->setNearClipDistance(0.1f);
	camera->setFarClipDistance(600.f);
	camera->setAutoAspectRatio(true);

	camera->setPosition(Vector3(0, 5, -20));
	camera->lookAt(Vector3(0, 3, 0));

	sceneMgr->getRootSceneNode()->attachObject(camera);
}
