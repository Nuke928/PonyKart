#include "pch.h"
#include "Core/Cameras/CameraManager.h"
#include "Kernel/LKernel.h"
#include "Levels/LevelManager.h"
#include <OgreViewport.h>

using namespace std;
using namespace Ponykart::Core;
using namespace Ponykart::Levels;
using namespace Ponykart::LKernel;

CameraEvent CameraManager::onPreCameraSwitch;
CameraEvent CameraManager::onPostCameraSwitch;
CameraEvent CameraManager::onCameraRegistration;

CameraManager::CameraManager()
{
	LevelManager::onLevelLoad.push_back(bind(&CameraManager::onLevelLoad,this,placeholders::_1));
	LevelManager::onLevelUnload.push_back(bind(&CameraManager::onLevelUnload,this,placeholders::_1));
}

/// \todo Implement this.
void CameraManager::onLevelLoad(LevelChangedEventArgs *args)
{
	//TODO: Implement this.
	//BasicCamera basicCamera = new BasicCamera("BasicCamera");
	//cameras.Add(basicCamera);
	//SwitchCurrentCamera(basicCamera);	
}

void CameraManager::onLevelUnload(LevelChangedEventArgs *args)
{
	typedef std::vector<LCamera*>::iterator itr_t;
	for(itr_t itr = cameras.begin(); itr != cameras.end(); itr++) {
		delete (*itr);
	}
	cameras.clear();
	currentCamera = nullptr;
}

void CameraManager::registerCamera(LCamera *camera)
{	
	if(!camera) {
		throw std::string("Tried to register a camera that was NULL!");
	}

	typedef std::vector<LCamera*>::iterator itr_t;
	for(itr_t itr = cameras.begin(); itr != cameras.end(); itr++) {
		if((*itr) == camera) {
			throw std::string("Tried to register a camera that was already registered!");
		} else if((*itr)->getName() == camera->getName()) {
			throw std::string("There is already a camera registered with that name!");
		}
	}

	cameras.push_back(camera);

	if(currentCamera == nullptr) {
		switchCurrentCamera(camera);
	}
	
	runCameraRegistrationEvent(camera);
}

void CameraManager::switchCurrentCamera(LCamera *camera)
{
	if(std::find(cameras.begin(),cameras.end(),camera)!=cameras.end()) {
		throw std::string("Tried to switch to a camera that wasn't registered to the CameraManager!");
	}

	runPreCameraSwitchEvent(camera);
	
	if(currentCamera != nullptr) {
		currentCamera->onSwitchToInactive(camera);
	}

	LCamera *oldCamera = currentCamera;
	currentCamera = camera;
	getG<Ogre::Viewport>()->setCamera(camera->camera);

	camera->onSwitchToActive(oldCamera);

	runPostCameraSwitchEvent(camera);
}

void CameraManager::runCameraRegistrationEvent(LCamera *camera)
{
	typedef std::vector<std::function<void (LCamera*)>>::iterator itr_t;
	for(itr_t itr = onCameraRegistration.begin(); itr != onCameraRegistration.end(); itr++) {
		(*itr)(camera);	
	}
}

void CameraManager::runPreCameraSwitchEvent(LCamera *camera)
{
	typedef std::vector<std::function<void (LCamera*)>>::iterator itr_t;
	for(itr_t itr = onPreCameraSwitch.begin(); itr != onPreCameraSwitch.end(); itr++) {
		(*itr)(camera);	
	}
}

void CameraManager::runPostCameraSwitchEvent(LCamera *camera)
{
	typedef std::vector<std::function<void (LCamera*)>>::iterator itr_t;
	for(itr_t itr = onPostCameraSwitch.begin(); itr != onPostCameraSwitch.end(); itr++) {
		(*itr)(camera);	
	}
}

const LCamera* const CameraManager::getCurrentCamera()
{
	return currentCamera;
}
