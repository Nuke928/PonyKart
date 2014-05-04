
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
	renderer = &(CEGUI::OgreRenderer::bootstrapSystem(renderWindow));
	CEGUI::SchemeManager::getSingleton().createFromFile("Generic.scheme");
	CEGUI::SchemeManager::getSingleton().createFromFile("menu.scheme");
	CEGUI::Window *mainWindow = CEGUI::WindowManager::getSingleton().createWindow("Generic/Image", "MainWindow");
	mainWindow->setSize(USize(UDim(1, 0), UDim(1, 0)));
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(mainWindow);

	// Add a quick menu placeholder as a test
	CEGUI::Window *dashbackground = CEGUI::WindowManager::getSingleton().createWindow("Generic/Image", "dashbackground");
	dashbackground->setSize(USize(UDim(1, 0), UDim(1, 0)));
	dashbackground->setProperty("Image", "dashbackground/dashbackground");
	mainWindow->addChild(dashbackground);

	CEGUI::Window *checkers = CEGUI::WindowManager::getSingleton().createWindow("Generic/Image", "checkers");
	checkers->setSize(USize(UDim(1,0), UDim(1, 0)));
	checkers->setProperty("Image", "checkers/checkers");
	mainWindow->addChild(checkers);
}

bool UIMain::frameStarted(const Ogre::FrameEvent& evt)
{
	/// TODO: Implement
	return true;
}