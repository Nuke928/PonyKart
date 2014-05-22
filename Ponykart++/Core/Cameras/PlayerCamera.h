#include "Core/Cameras/LCamera.h"

namespace Ponykart
{
namespace Actors { class Kart; }
namespace Core
{
	/// A basic third-person camera with some smoothing.
	class PlayerCamera : public LCamera
	{
	public:
		PlayerCamera(const std::string& name);

	protected:
		virtual bool updateCamera(const Ogre::FrameEvent& evt) override;
		btDynamicsWorld::ClosestRayResultCallback castRay(Ogre::Vector3 derivedCam, Ogre::Vector3 derivedTarget); ///< cast a ray from the target position to the camera position

	protected:
		const static float _cameraTightness;
		const static float _cameraTargetYOffset;

	private:
		Ogre::SceneNode* targetNode;
		Actors::Kart* followKart;
		Ogre::SceneNode* kartCamNode;
		Ogre::SceneNode* kartTargetNode;
		float rayLength;
		btDiscreteDynamicsWorld* world;
	};
}
}