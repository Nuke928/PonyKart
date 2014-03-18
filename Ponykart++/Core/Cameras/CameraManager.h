#ifndef CAMERAMANAGER_H_INCLUDED
#define CAMERAMANAGER_H_INCLUDED

#include <functional>
#include <vector>
#include "Core/Cameras/LCamera.h"
#include "Kernel/LKernelObject.h"
#include "Levels/LevelManager.h"

namespace Ponykart
{
namespace Core
{

using CameraEvent = std::vector<std::function<void (LCamera *camera)>>;

/// Manages all of our cameras and handles switching between them as necessary.
class CameraManager : public LKernel::LKernelObject
{
public:
	CameraManager();

	void onLevelLoad(Levels::LevelChangedEventArgs *args); ///< Create a temporary camera for loading process
	void onLevelUnload(Levels::LevelChangedEventArgs *args); ///< Delete all cameras and clear the list

	//! Registers a new camera
	/*!
	 * The camera must not've already been registered.
	 * If we aren't using a camera yet, this will switch our rendering to use it.
	 */
	void registerCamera(LCamera *camera);

	void switchCurrentCamera(LCamera *camera); ///< Switch rendering to another camera. It has to be registered.

	// getters
	const LCamera* const getCurrentCamera(); ///< Gets the current camera that is being used for rendering.

public:
	std::vector<LCamera*> cameras; ///< List of all cameras

private:
	LCamera* currentCamera; ///< The current camera

	void runCameraRegistrationEvent(LCamera *camera); ///< Runs all events listed in onCameraRegistration with camera
	void runPreCameraSwitchEvent(LCamera *camera); ///< Runs all events listed in onPreCameraSwitchEvent with camera
	void runPostCameraSwitchEvent(LCamera *camera); ///< Runs all events listed in onPostCameraSwitchEvent with camera

public:
	static CameraEvent onPreCameraSwitch; ///< Fired when we switch between cameras
	static CameraEvent onPostCameraSwitch; ///< Fired when we switch between cameras
	static CameraEvent onCameraRegistration; ///< Fired when a camera is registered
};
} // Core
} // Ponykart

#endif // CAMERAMANAGER_H_INCLUDED
