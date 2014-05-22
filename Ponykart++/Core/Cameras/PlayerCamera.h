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