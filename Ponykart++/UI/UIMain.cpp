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
	/// TODO: Implement properly

	// Initialize CEGUI
	Ogre::RenderWindow& renderWindow = *LKernel::getG<Ogre::RenderWindow>();
	renderer = &(OgreRenderer::bootstrapSystem(renderWindow));
	SchemeManager::getSingleton().createFromFile("Generic.scheme");
	SchemeManager::getSingleton().createFromFile("menu.scheme");
	Window *mainWindow = WindowManager::getSingleton().createWindow("Generic/Image", "MainWindow");
	mainWindow->setSize(USize(UDim(1, 0), UDim(1, 0)));
	System::getSingleton().getDefaultGUIContext().setRootWindow(mainWindow);
}

bool UIMain::frameStarted(const Ogre::FrameEvent& evt)
{
	System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);
	return true;
}