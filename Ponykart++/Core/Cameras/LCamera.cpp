#include "pch.h"
#include <string>
#include <OgreCamera.h>
#include <OgreRoot.h>
#include <OgreSceneNode.h>
#include "Core/Cameras/CameraManager.h"
#include "Core/Cameras/LCamera.h"
#include "Kernel/LKernel.h"
#include "Physics/PhysicsMain.h"

using namespace Ponykart::Core;
using namespace Ponykart::LKernel;
using namespace Ponykart::Physics;
using namespace Ogre;
using namespace std;

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
    //getG<Root>()->addFrameListener(this);
    PhysicsMain::postSimulate.push_back(bind(&LCamera::postSimulate, this, placeholders::_1, placeholders::_2));
}

void LCamera::onSwitchToInactive(LCamera *oldCamera)
{
	isActive = false;

    using fnType = void(btDiscreteDynamicsWorld* world, const FrameEvent& evt);
    using stdFnType = std::function<fnType>;

    //getG<Root>()->removeFrameListener(this);
    auto localFn = stdFnType(bind(&LCamera::postSimulate, this, placeholders::_1, placeholders::_2));
    auto it = find_if(begin(PhysicsMain::postSimulate), end(PhysicsMain::postSimulate),
        [&](stdFnType fn) {return fn.target<fnType>() == localFn.target<fnType>(); });
    if (it != end(PhysicsMain::postSimulate))
        PhysicsMain::postSimulate.erase(it);
    // TODO: Check that the above works correctly
}

void LCamera::registerCam()
{
	getG<CameraManager>()->registerCamera(this);
}

void LCamera::makeActiveCamera()
{
	getG<CameraManager>()->switchCurrentCamera(this);
}

bool LCamera::frameRenderingQueued(const FrameEvent& evt)
{
	return updateCamera(evt);
}

bool LCamera::postSimulate(btDiscreteDynamicsWorld* world, const Ogre::FrameEvent& evt)
{
    return updateCamera(evt);
}
