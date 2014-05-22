#ifndef BASICCAMERA_H_INCLUDED
#define BASICCAMERA_H_INCLUDED

#include "Core/Cameras/LCamera.h"

namespace Ponykart
{
namespace Core
{
	class BasicCamera : public LCamera
	{
	public:
		BasicCamera(const std::string& name);
		virtual bool updateCamera(const Ogre::FrameEvent& evt) override;
	};
}
}

#endif BASICCAMERA_H_INCLUDED
