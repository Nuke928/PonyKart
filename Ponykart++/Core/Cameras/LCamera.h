#ifndef LCAMERA_H_INCLUDED
#define LCAMERA_H_INCLUDED

#include <string>

namespace Ogre
{
	struct FrameEvent;
	class Camera;
	class SceneNode;
}

class btDiscreteDynamicsWorld;
namespace Ponykart
{
namespace Core
{
/// All camera classes should subclass from this.
class LCamera : Ogre::FrameListener
{
public:
	LCamera(const std::string& Name);
	void registerCam(); ///< Shortand
	void makeActiveCamera(); ///< Make sure you register the camera before calling this!
	// Getters
	const Ogre::Camera* const getCamera() const; ///< Gets the Ogre camera
	const Ogre::SceneNode* const getCameraNode() const; ///< Gets the scene node
	std::string getName() const; ///< Gets the name of the camera

	virtual void onSwitchToActive(LCamera *oldCamera); ///< Is ran when we switch cameras and this one becomes the active camera
	virtual void onSwitchToInactive(LCamera *newCamera); ///< Is ran when we switch from this camera to another camera

protected:
	virtual bool updateCamera(const Ogre::FrameEvent& evt) = 0; ///< Called every frame when the camera is the active camera
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt) override; ///< Calls updateCamera
    virtual bool postSimulate(btDiscreteDynamicsWorld* world, const Ogre::FrameEvent& evt); ///< Calls updateCamera

protected:
	friend class CameraManager; ///< CameraManager needs to access the Ogre camera

	Ogre::Camera* camera; ///< The Ogre camera we're manipulating
	Ogre::SceneNode* cameraNode; ///< The scene node the camera is attached to
	std::string name; ///< The name of this camera
	bool isActive; ///< Is this camera active or not?
};
} // Core
} // Ponykart

#endif // LCAMERA_H_INCLUDED
