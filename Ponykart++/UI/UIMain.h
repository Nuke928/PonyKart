#ifndef UIMAIN_H_INCLUDED
#define UIMAIN_H_INCLUDED

#include <OgreFrameListener.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>
#include "Kernel/LKernelObject.h"

namespace Ponykart
{
namespace UI
{
	/// The class that manages the UI system
	class UIMain : public LKernel::LKernelObject, public Ogre::FrameListener
	{
	public:
		UIMain();
		void changeSceneManager(Ogre::SceneManager* newSceneManager);

	private:
		bool frameStarted(const Ogre::FrameEvent& evt) override;

	private:
		CEGUI::OgreRenderer* renderer;
	};
}
}

#endif