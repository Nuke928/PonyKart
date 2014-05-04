
#include <OgreRenderWindow.h>
#include "pch.h"
#include "UI/UIMain.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelOgre.h"

using namespace Ponykart::LKernel;
using namespace Ponykart::UI;
using namespace CEGUI;

UIMain::UIMain()
{
	/// TODO: Implement
	Ogre::RenderWindow& window = *LKernel::getG<Ogre::RenderWindow>();
	renderer = &(CEGUI::OgreRenderer::bootstrapSystem(window));
}

bool UIMain::frameStarted(const Ogre::FrameEvent& evt)
{
	/// TODO: Implement
	return true;
}